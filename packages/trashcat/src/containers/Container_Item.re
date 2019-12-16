open Externals;
open Lib.Styles;

module GetItemFragment = [%graphql
  {|
    fragment itemFragment on Item {
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
    }
  |}
];

type data = {src: VideoSurface.src};

type state =
  | Loading
  | Error
  | Data(data);

[@react.component]
let make = (~itemFragment as item, ~autoPlay=false, ~style=?) => {
  let s3Objects =
    item##video##files
    ->Belt.Array.map(i => i##file->S3Object.fromJs)
    ->Js.Option.some
    ->Hook.S3ObjectResolver.use;

  let state =
    switch (s3Objects) {
    | Error => Error
    | Resolved(d) =>
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
      Data({src: srcElements->VideoSurface.srcElement});
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
  | Data({src}) =>
    <div
      ?style
      className={cn(["w-screen", "h-screen", "relative", "overflow-hidden"])}>
      <VideoSurface src autoPlay />
    </div>
  | Error => React.string("Nothing here!")
  };
};
