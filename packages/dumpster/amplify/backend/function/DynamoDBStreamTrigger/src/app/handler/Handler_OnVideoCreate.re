open Externals;
open Lib;

[@bs.deriving jsConverter]
type error = [
  | [@bs.as "Invalid Item Error: Video already processed."]
    `InvalidItem_VideoAlreadyProcessed
  | [@bs.as "Invalid Item Error: Missing video source."]
    `InvalidItem_MissingVideoSource
  | [@bs.as "Invalid Item Error: Unsupported object path."]
    `InvalidItem_UnsupportedObjectPath
  | [@bs.as "Media Convert Error: Failed to create job."]
    `MediaConvert_CreateJobFailure
  | [@bs.as "GraphQL Error: API Error"] `GraphQL_APIError
];

module CreateMediaConvertJobMutation = [%graphql
  {|
    mutation CreateMediaConvertJob($input: CreateMediaConvertJobInput!) {
      createMediaConvertJob(input: $input) {
        id
      }
    }
  |}
];

module ListMediaConvertJobsQuery = [%graphql
  {|
    query ListMediaConvertJobs($mediaConvertJobVideoId: ID!) {
      mediaConvertJobsByVideoId(
        mediaConvertJobVideoId: $mediaConvertJobVideoId
      ) {
        items {
          id
        }
      }
    }
  |}
];

let handle = r => {
  let video =
    r
    ->DynamoDBStreamRecord.dynamodbGet
    ->DynamoDBStreamRecord.VideoRecord.newImageGet;
  let mediaConvertJobQuery =
    ListMediaConvertJobsQuery.make(
      ~mediaConvertJobVideoId=video##id->DynamoDBStreamRecord.StringField.get,
      (),
    );

  AwsAmplify.Api.graphqlOperation(
    ~query=mediaConvertJobQuery##query,
    ~variables=mediaConvertJobQuery##variables,
  )
  |> AwsAmplify.Api.(graphql(inst))
  |> Js.Promise.then_(r => {
       let queryResult = r->ListMediaConvertJobsQuery.parse;
       switch (
         queryResult##mediaConvertJobsByVideoId->Belt.Option.flatMap(i => i##items),
         video##files
         ->DynamoDBStreamRecord.ArrayField.get
         ->Belt.Array.get(0)
         ->Belt.Option.map(DynamoDBStreamRecord.MapField.get),
       ) {
       | (None, Some(f)) =>
         let s3Object =
           S3Object.make(
             ~key=
               DynamoDBStreamRecord.(
                 f##file->MapField.get->(f => f##key)->StringField.get
               ),
             ~bucket=
               DynamoDBStreamRecord.(
                 f##file->MapField.get->(f => f##bucket)->StringField.get
               ),
           );
         switch (s3Object->S3Object.namePathPartGet) {
         | Some(namePath) =>
           let job =
             MediaConvertJob.make(
               ~iamRole=Constants.Env.mediaConvertIamRoleArn,
               ~sourceS3Object=s3Object,
               ~destinationS3Object=
                 S3Object.make(
                   ~bucket=s3Object->S3Object.bucketGet,
                   ~key="public/item-video/",
                 ),
             );
           Constants.mediaConvertService
           ->AWSSDK.MediaConvert.createJob(job)
           ->AWSSDK.Request.promise
           |> Js.Promise.then_(createdJob => {
                let mutation =
                  CreateMediaConvertJobMutation.make(
                    ~input={
                      "state": `SUBMITTED,
                      "mediaConvertJobVideoId":
                        video##id->DynamoDBStreamRecord.StringField.get,
                      "externalId":
                        createdJob
                        ->AWSSDK.MediaConvert.Job.jobGet
                        ->AWSSDK.MediaConvert.Job.idGet,
                      "id": UUID.makeV4(),
                    },
                    (),
                  );
                AwsAmplify.Api.(
                  graphqlOperation(
                    ~query=mutation##query,
                    ~variables=mutation##variables,
                  )
                  |> graphql(inst)
                )
                |> Js.Promise.then_(r => {
                     let _mutationResult =
                       r->CreateMediaConvertJobMutation.parse;
                     Js.Promise.resolve(Belt.Result.Ok());
                   })
                |> Js.Promise.catch(err => {
                     Js.log(err);
                     Js.Promise.resolve(
                       Belt.Result.Error(`GraphQL_APIError->errorToJs),
                     );
                   });
              })
           |> Js.Promise.catch(err => {
                Js.log(err);
                Js.Promise.resolve(
                  Belt.Result.Error(
                    `MediaConvert_CreateJobFailure->errorToJs,
                  ),
                );
              });
         | None =>
           Js.Promise.resolve(
             Belt.Result.Error(`InvalidItem_UnsupportedObjectPath->errorToJs),
           )
         };
       | (None, None) =>
         Js.Promise.resolve(
           Belt.Result.Error(`InvalidItem_MissingVideoSource->errorToJs),
         )
       | (Some(_videoMediaConvertJobId), _) =>
         Js.Promise.resolve(
           Belt.Result.Error(`InvalidItem_VideoAlreadyProcessed->errorToJs),
         )
       };
     })
  |> Js.Promise.catch(err => {
       Js.log(err);
       Js.Promise.resolve(Belt.Result.Error(`GraphQL_APIError->errorToJs));
     });
};
