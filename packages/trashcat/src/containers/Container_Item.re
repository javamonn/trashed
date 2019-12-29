open Externals;
open Lib.Styles;

[@bs.deriving jsConverter]
type error = [
  | [@bs.as "InvalidState_FailedS3Resolution"] `InvalidState_FailedS3Resolution
];

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
        poster {
          bucket
          key
          region
        }
      }
      location {
        lat
        lon
      }
      createdAt
    }
  |}
];

type data = {
  src: VideoSurface.src,
  poster: option(string),
};

type state =
  | Loading
  | Error
  | Data(data);

[@react.component]
let make = (~itemFragment as item, ~autoPlay=false, ~style=?) => {
  let (hasPoster, s3Objects) = {
    let videoFileS3Objects =
      item##video##files->Belt.Array.map(i => i##file->S3Object.fromJs);
    let posterS3Object =
      item##video##poster->Belt.Option.map(S3Object.fromJs);
    switch (posterS3Object) {
    | Some(posterS3Object) => (
        true,
        Belt.Array.concat([|posterS3Object|], videoFileS3Objects),
      )
    | None => (false, videoFileS3Objects)
    };
  };
  let resolvedS3Objects = s3Objects->Js.Option.some->Hook.S3ObjectResolver.use;

  let state =
    switch (resolvedS3Objects) {
    | Error => Error
    | Resolved(data) =>
      let (poster, videos) =
        if (hasPoster) {
          (data->Belt.Array.get(0), data->Belt.Array.sliceToEnd(1));
        } else {
          (None, data);
        };
      let srcElements =
        Belt.Array.zip(videos, item##video##files)
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
      Data({src: srcElements->VideoSurface.srcElement, poster});
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
  | Data({src, poster}) =>
    <div
      ?style
      className={cn(["w-screen", "h-screen", "relative", "overflow-hidden"])}>
      <VideoSurface src autoPlay ?poster />
      <ItemBottomOverlay
        location=(item##location##lat, item##location##lon)
        createdAt={item##createdAt}
      />
    </div>
  | Error => `InvalidState_FailedS3Resolution->errorToJs->Js.Exn.raiseError
  };
};
