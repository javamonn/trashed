open Lib;

[@bs.deriving jsConverter]
type error = [
  | [@bs.as "Invalid Item Error: Missing Media Convert Job."]
    `InvalidItem_MissingMediaConvertJob
];

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
         Belt.Result.Error(`InvalidItem_MissingMediaConvertJob)
         ->Js.Promise.resolve
       }
     );
};

let handle = jobId => {
  getMediaConvertJob(jobId)
  ->Utils.pThen(item =>
      item
      ->Belt.Result.map(item =>
          GraphQL.updateMediaConvertJob({
            "id": item##id,
            "externalId": Some(item##externalId),
            "mediaConvertJobVideoId": Some(item##mediaConvertJobVideoId),
            "state": Some(`COMPLETE),
          })
        )
      ->Js.Promise.resolve
    )
  ->Utils.pThen(r => r->Belt.Result.map(_ => ())->Js.Promise.resolve);
};
