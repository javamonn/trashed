open Lib.Styles;
open Externals;

module GetItemQueryConfig = [%graphql
  {|
    query GetItem($itemId: ID!) {
      getItem(id: $itemId) {
        id
        video {
          id
          files {
            file {
              bucket
              key
              region
            }
            mimeType
          }
        }
        location {
          lat
          lon
        }
      }
    }
  |}
];

module GetItemQuery = ReasonApolloHooks.Query.Make(GetItemQueryConfig);

type state =
  | Loading
  | Error
  | Data(VideoSurface.src);

[@react.component]
let make = (~itemId) => {
  let (query, _fullQuery) =
    GetItemQuery.use(
      ~variables=GetItemQueryConfig.make(~itemId, ())##variables,
      (),
    );
  let files =
    query
    ->(
        fun
        | ReasonApolloHooks.Query.Loading
        | Error(_)
        | NoData => None
        | Data(d) => Some(d)
      )
    ->Belt.Option.flatMap(i => i##getItem)
    ->Belt.Option.map(i => i##video##files);

  let s3Objects =
    Hook.S3ObjectResolver.useMany(
      files->Belt.Option.map(i =>
        i->Belt.Array.map(i => i##file->S3Object.fromJs)
      ),
    );

  let state =
    switch (query, s3Objects, files) {
    | (Error(_), _, _)
    | (_, Error, _) => Error
    | (_, Resolved(d), Some(files)) =>
      Data(
        Belt.Array.zip(d, files)
        ->Belt.Array.map(((src, file)) => (src, file##mimeType))
        ->VideoSurface.srcElement,
      )
    | _ => Loading
    };

  switch (state) {
  | Loading =>
    <div
      className={cn([
        "w-screen",
        "h-screen",
        "flex",
        "justify-center",
        "items-center",
        "flex-col",
      ])}>
      <Progress />
    </div>
  | Data(src) =>
    <div className={cn(["w-screen", "h-screen", "relative"])}>
      <VideoSurface src autoPlay=true controls=true />
    </div>
  | Error => React.string("Nothing here!")
  };
};
