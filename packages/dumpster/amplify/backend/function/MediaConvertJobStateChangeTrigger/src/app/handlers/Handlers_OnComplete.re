open Lib;
open Externals;

[@bs.deriving jsConverter]
type error = [
  | [@bs.as "Unable to get mediaConvertJob from GraphQL."]
    `GraphQL_UnableToGetMediaConvertJob
  | [@bs.as "Mutation Error: UpdateVideo failed."]
    `GraphQL_MutationError_UpdateVideo
  | [@bs.as "Mutation Error: UpdateMediaConvertJob failed."]
    `GraphQL_MutationError_UpdateMediaConvertJob
  | [@bs.as "Unable to get mediaConvertJob from AWS."]
    `AWS_UnableToGetMediaConvertJob
];

[@bs.deriving jsConverter]
type mimeType = [ | [@bs.as "WEBM"] `WEBM | [@bs.as "MP4"] `MP4];

module ListMediaConvertJobsQuery = [%graphql
  {|
    query ListMediaConvertJobs($externalId: ID!) {
      listMediaConvertJobs(
        filter: {
          externalId: {
            eq: $externalId
          }
        },
        limit: 1
      ) {
        items {
          id
          state
          mediaConvertJobVideoId
          externalId
          video {
            id
            files {
              file {
                bucket
                key
                region
              }
              mimeType
              userUpload
              cloudfrontUrl
            }
            thumbnail {
              bucket
              key
              region
            }
          }
        }
      }
    }
  |}
];

let getMediaConvertJob = externalId => {
  let mediaConvertJobQuery = ListMediaConvertJobsQuery.make(~externalId, ());
  AwsAmplify.Api.graphqlOperation(
    ~query=mediaConvertJobQuery##query,
    ~variables=mediaConvertJobQuery##variables,
  )
  |> AwsAmplify.Api.(graphql(inst))
  |> Js.Promise.then_(r =>
       switch (
         r
         ->ListMediaConvertJobsQuery.parse
         ->(r => r##listMediaConvertJobs)
         ->Belt.Option.flatMap(r => r##items)
         ->Belt.Option.flatMap(r =>
             r->Belt.Array.get(0)->Belt.Option.flatMap(Utils.identity)
           )
       ) {
       | Some(item) => Belt.Result.Ok(item)->Js.Promise.resolve
       | None =>
         Belt.Result.Error(`GraphQL_UnableToGetMediaConvertJob)
         ->Js.Promise.resolve
       }
     );
};

let getDestinationS3Object = jobId =>
  Constants.mediaConvertService
  ->AWSSDK.MediaConvert.(getJob(getJobParams(~id=jobId)))
  ->AWSSDK.Request.promise
  ->Utils.Promise.then_(jobResponse => {
      let mimeTypes =
        AWSSDK.MediaConvert.Job.(
          jobResponse
          ->jobGet
          ->settingsGet
          ->outputsGet
          ->Belt.Array.map(o =>
              o->containerSettingsGet->containerGet->mimeTypeFromJs
            )
        );
      let destinationPaths =
        AWSSDK.MediaConvert.Job.(
          jobResponse
          ->jobGet
          ->settingsGet
          ->outputGroupsGet
          ->Belt.Array.map(og =>
              og
              ->outputGroupSettingsGet
              ->fileGroupSettingsGet
              ->destinationGet
              ->S3Object.fromString
              ->Js.Option.getExn
            )
        );
      Belt.Array.zip(mimeTypes, destinationPaths)
      ->Belt.Array.keep(((mimeType, _)) => Belt.Option.isSome(mimeType))
      ->Belt.Array.map(((mimeType, s3Object)) =>
          {
            "file": {
              "bucket": s3Object->S3Object.bucketGet,
              "key": s3Object->S3Object.keyGet,
              "region": Constants.Env.region,
              "localUri": None,
              "mimeType": None,
            },
            "mimeType": mimeType->Belt.Option.getExn,
            "userUpload": false,
            "cloudfrontUrl": None,
          }
        )
      ->Js.Promise.resolve;
    })
  ->Utils.Promise.asResult(_err => `AWS_UnableToGetMediaConvertJob);

let handle = jobId => {
  Js.Promise.all2((
    getMediaConvertJob(jobId),
    getDestinationS3Object(jobId),
  ))
  ->Utils.Promise.then_(r =>
      switch (r->Utils.Result.lift2) {
      | Belt.Result.Ok((mediaConvertJob, videoObject)) =>
        let pUpdateMediaConvertJob =
          GraphQL.updateMediaConvertJob({
            "id": mediaConvertJob##id,
            "externalId": Some(mediaConvertJob##externalId),
            "mediaConvertJobVideoId":
              Some(mediaConvertJob##mediaConvertJobVideoId),
            "state": Some(`COMPLETE),
          })
          ->Utils.Promise.asResult(_err =>
              `GraphQL_MutationError_UpdateMediaConvertJob
            );
        let pUpdateVideo =
          GraphQL.updateVideo({
            "id": mediaConvertJob##video##id,
            "files":
              mediaConvertJob##video##files
              ->Belt.Array.map(f =>
                  {
                    "file": {
                      "bucket": f##file##bucket,
                      "key": f##file##key,
                      "region": f##file##region,
                      "localUri": None,
                      "mimeType": None,
                    },
                    "mimeType": f##mimeType,
                    "userUpload": f##userUpload,
                    "cloudfrontUrl": f##cloudfrontUrl,
                  }
                )
              ->Belt.Array.concat(videoObject)
              ->Js.Option.some,
            "thumbnail": None,
          })
          ->Utils.Promise.asResult(_err => `GraphQL_MutationError_UpdateVideo);
        Js.Promise.all2((pUpdateMediaConvertJob, pUpdateVideo))
        ->Utils.Promise.then_(p => p->Utils.Result.lift2->Js.Promise.resolve);
      | Belt.Result.Error(e) as err => Js.Promise.resolve(err)
      }
    )
  ->Utils.Promise.then_(r => r->Belt.Result.map(_ => ())->Js.Promise.resolve);
};
