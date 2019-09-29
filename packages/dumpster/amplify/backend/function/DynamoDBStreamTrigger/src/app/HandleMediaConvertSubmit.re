open Externals;
open Lib;

[@bs.deriving jsConverter]
type error = [
  | [@bs.as "Invalid Item: Video already processed."]
    `InvalidItem_VideoAlreadyProcessed
  | [@bs.as "Invalid Item: Missing video source."]
    `InvalidItem_MissingVideoSource
  | [@bs.as "Invalid Item: Unsupported object path."]
    `InvalidItem_UnsupportedObjectPath
  | [@bs.as "Media Convert: Failed to create job."]
    `MediaConvert_CreateJobFailure
  | [@bs.as "GraphQL: API Error"] `GraphQL_APIError
];

module CreateMediaConvertJobMutationConfig = [%graphql
  {|
    mutation CreateMediaConvertJob($input: CreateMediaConvertJobInput!) {
      createMediaConvertJob(input: $input) {
        id
      }
    }
  |}
];

/**
 * FIXME: Can connection be one-way from MediaConvertJob? Else we have to
 * update Video with the created mediacConvertJobId as well.
 *
 * Store AWS MediaConvertJob ID on API MediaConvertJob
 */
let handle = r => {
  let video =
    r
    ->DynamoDBStreamRecord.dynamodbGet
    ->DynamoDBStreamRecord.VideoRecord.newImageGet;
  switch (
    video##videoMediaConvertJobId
    ->DynamoDBStreamRecord.NullableStringField.get,
    video##files->DynamoDBStreamRecord.ArrayField.get->Belt.Array.get(0),
  ) {
  | (None, Some(f)) =>
    let s3Object =
      S3Object.make(
        ~key=f##file##key->DynamoDBStreamRecord.StringField.get,
        ~bucket=f##file##bucket->DynamoDBStreamRecord.StringField.get,
      );
    switch (s3Object->S3Object.namePathPartGet) {
    | Some(namePath) =>
      let job =
        MediaConvertJob.make(
          ~iamRole=Constants.awsMediaConvertIamRole,
          ~sourceS3Object=s3Object,
          ~destinationS3Object=
            S3Object.make(
              ~bucket="trashcat",
              ~key="public/item-video" ++ namePath,
            ),
        );
      AWSSDK.MediaConvert.(service->createJob(job)->promise)
      |> Js.Promise.then_(jobResult => {
           let mutation =
             CreateMediaConvertJobMutationConfig.make(
               ~input={
                 "state": `SUBMITTED,
                 "mediaConvertJobVideoId":
                   video##id->DynamoDBStreamRecord.StringField.get,
                 "id": None,
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
                let mutationResult =
                  r->CreateMediaConvertJobMutationConfig.parse;
                Js.Promise.resolve(Belt.Result.Ok());
              })
           |> Js.Promise.catch(_err =>
                Js.Promise.resolve(Belt.Result.Error(`GraphQL_APIError))
              );
         })
      |> Js.Promise.catch(err =>
           Js.Promise.resolve(
             Belt.Result.Error(`MediaConvert_CreateJobFailure),
           )
         );
    | None =>
      Js.Promise.resolve(
        Belt.Result.Error(`InvalidItem_UnsupportedObjectPath),
      )
    };
  | (None, None) =>
    Js.Promise.resolve(Belt.Result.Error(`InvalidItem_MissingVideoSource))
  | (Some(_videoMediaConvertJobId), _) =>
    Js.Promise.resolve(Belt.Result.Error(`InvalidItem_VideoAlreadyProcessed))
  };
};
