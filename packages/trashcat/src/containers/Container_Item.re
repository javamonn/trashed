open Externals;
open Lib.Styles;

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

type data = {
  src: VideoSurface.src,
  location: (float, float),
};

type state =
  | Loading
  | Error
  | Data(data);

[@react.component]
let make = (~itemId, ~autoPlay=false, ~style=?) => {
  let (query, _fullQuery) =
    GetItemQuery.use(
      ~variables=GetItemQueryConfig.make(~itemId, ())##variables,
      (),
    );
  let item =
    query
    ->(
        fun
        | ReasonApolloHooks.Query.Loading
        | Error(_)
        | NoData => None
        | Data(d) => Some(d)
      )
    ->Belt.Option.flatMap(i => i##getItem);

  let s3Objects =
    Hook.S3ObjectResolver.use(
      item
      ->Belt.Option.map(i => i##video##files)
      ->Belt.Option.map(i => i->Belt.Array.map(i => i##file->S3Object.fromJs)),
    );

  let state =
    switch (query, s3Objects, item) {
    | (Error(_), _, _)
    | (_, Error, _) => Error
    | (_, Resolved(d), Some(item)) =>
      let srcElements =
        Belt.Array.zip(d, item##video##files)
        ->Belt.Array.map(((src, file)) => (src, file##mimeType));
      let _ =
        Js.Array.sortInPlaceWith(
          ((_, mime1), (_, mime2)) =>
            switch (mime1, mime2) {
            | (`WEBM, `MP4) => (-1)
            | (`MP4, `WEBM) => 1
            | _ => 0
            },
          srcElements,
        );
      Data({
        src: srcElements->VideoSurface.srcElement,
        location: (item##location##lat, item##location##lon),
      });
    | _ => Loading
    };

  switch (state) {
  | Loading =>
    <div
      ?style
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
  | Data({src, location}) =>
    <div
      ?style
      className={cn(["w-screen", "h-screen", "relative", "overflow-hidden"])}>
      <VideoSurface src autoPlay />
    </div>
  | Error => React.string("Nothing here!")
  };
};
