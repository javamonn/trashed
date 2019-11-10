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
  | [@bs.as "Unable to parse trigger event."]
    `AWS_UnableToParseMediaConvertOutputs
  | [@bs.as "Unknown Error."] `Unknown_Error
];

module MediaConvertOutput = {
  [@bs.deriving jsConverter]
  type objectType = [
    | [@bs.as "full-video"] `FullVideo
    | [@bs.as "clip-thumbnail"] `ClipThumbnail
    | [@bs.as "clip-poster"] `ClipPoster
  ];

  [@bs.deriving abstract]
  type t = {
    objectType,
    s3Object: S3Object.t,
  };
  let make = t;
};

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
         ->Js.Json.decodeObject
         ->Belt.Option.flatMap(o => o->Js.Dict.get("data"))
         ->Belt.Option.flatMap(d =>
             d->ListMediaConvertJobsQuery.parse->(r => r##listMediaConvertJobs)
           )
         ->Belt.Option.flatMap(r => r##items)
         ->Belt.Option.flatMap(r =>
             r->Belt.Array.get(0)->Belt.Option.flatMap(Utils.identity)
           )
       ) {
       | Some(item) => Belt.Result.Ok(item)->Js.Promise.resolve
       | None =>
         Js.log(r->Js.Json.stringifyAny);
         Belt.Result.Error(`GraphQL_UnableToGetMediaConvertJob)
         ->Js.Promise.resolve;
       }
     );
};

let getMediaConvertOutputs = event =>
  event
  ->Lambda.Event.outputGroupDetailsGet
  ->Belt.Array.get(0)
  ->Belt.Option.map(ogd =>
      ogd
      ->Lambda.Event.outputDetailsGet
      ->Belt.Array.map(od => {
          let s3Object =
            od
            ->Lambda.Event.outputFilePathsGet
            ->Belt.Array.get(0)
            ->Belt.Option.flatMap(S3Object.fromString)
            ->Belt.Option.getExn;

          let keyParts = s3Object->S3Object.key->Js.String2.split("/");
          let lastPath = keyParts[Js.Array.length(keyParts) - 1];

          lastPath
          ->Js.String2.split(".")
          ->Belt.Array.get(0)
          ->Belt.Option.flatMap(MediaConvertOutput.objectTypeFromJs)
          ->Belt.Option.map(objectType =>
              MediaConvertOutput.make(~objectType, ~s3Object)
            );
        })
    )
  ->Belt.Option.map(a => a->Belt.Array.keepMap(Utils.identity))
  ->Utils.Result.fromOption(`AWS_UnableToParseMediaConvertOutputs);

let handle = eventDetail => {
  Js.log("Handlers_OnComplete");
  eventDetail
  ->Lambda.Event.jobIdGet
  ->getMediaConvertJob
  ->Utils.Promise.then_(r =>
      switch (Utils.Result.lift2((r, eventDetail->getMediaConvertOutputs))) {
      | Belt.Result.Ok((job, outputs)) =>
        let pUpdateMediaConvertJob =
          GraphQL.updateMediaConvertJob({
            "id": job##id,
            "externalId": Some(job##externalId),
            "mediaConvertJobVideoId": Some(job##mediaConvertJobVideoId),
            "state": Some(`COMPLETE),
          })
          ->Utils.Promise.asResult(err => {
              Js.log(err);
              `GraphQL_MutationError_UpdateMediaConvertJob;
            });
        let pUpdateVideo =
          outputs
          ->Belt.Array.reduce(
              {
                "id": job##video##id,
                "files":
                  job##video##files
                  ->Belt.Array.map(m =>
                      {
                        "file": {
                          "key": m##file##key,
                          "bucket": m##file##bucket,
                          "region": m##file##region,
                          "localUri": None,
                          "mimeType": None,
                        },
                        "mimeType": m##mimeType,
                        "userUpload": m##userUpload,
                        "cloudfrontUrl": m##cloudfrontUrl,
                      }
                    )
                  ->Js.Option.some,
                "thumbnail": None,
                "poster": None,
              },
              (video, output) =>
              switch (output->MediaConvertOutput.objectTypeGet) {
              | `FullVideo => {
                  "id": video##id,
                  "files":
                    video##files
                    ->Belt.Option.map(
                        Belt.Array.concat([|
                          {
                            "file":
                              output
                              ->MediaConvertOutput.s3ObjectGet
                              ->S3Object.toObjectInput,
                            "mimeType":
                              output
                              ->MediaConvertOutput.s3ObjectGet
                              ->S3Object.mimeTypeGet
                              ->Belt.Option.flatMap(
                                  fun
                                  | `MP4 => Some(`MP4)
                                  | _ => None,
                                )
                              ->Belt.Option.getExn,
                            "userUpload": false,
                            "cloudfrontUrl": None,
                          },
                        |]),
                      ),
                  "poster": video##poster,
                  "thumbnail": video##thumbnail,
                }
              | `ClipThumbnail => {
                  "id": video##id,
                  "files": video##files,
                  "poster": video##poster,
                  "thumbnail":
                    output
                    ->MediaConvertOutput.s3ObjectGet
                    ->S3Object.toObjectInput
                    ->Js.Option.some,
                }
              | `ClipPoster => {
                  "id": video##id,
                  "files": video##files,
                  "thumbnail": video##thumbnail,
                  "poster":
                    output
                    ->MediaConvertOutput.s3ObjectGet
                    ->S3Object.toObjectInput
                    ->Js.Option.some,
                }
              }
            )
          ->GraphQL.updateVideo
          ->Utils.Promise.asResult(_err => `GraphQL_MutationError_UpdateVideo);
        Js.Promise.all2((pUpdateMediaConvertJob, pUpdateVideo))
        ->Utils.Promise.then_(p => p->Utils.Result.lift2->Js.Promise.resolve);
      | Belt.Result.Error(_e) as err => Js.Promise.resolve(err)
      }
    )
  ->Utils.Promise.then_(r =>
      switch (r) {
      | Belt.Result.Ok(r) =>
        Js.log(Js.Json.stringifyAny(r));
        Belt.Result.Ok()->Js.Promise.resolve;
      | Belt.Result.Error(e) =>
        e->errorToJs->Utils.Result.error->Js.Promise.resolve
      }
    )
  ->Utils.Promise.catch_(err => {
      Js.log(err);
      `Unknown_Error->errorToJs->Utils.Result.error->Js.Promise.resolve;
    });
};
