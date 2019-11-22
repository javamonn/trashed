open Externals;
open Lib.Styles;
open Lib.Utils;

[@bs.deriving jsConverter]
type mimeType = [ | [@bs.as "video/webm"] `WEBM | [@bs.as "video/mp4"] `MP4];

[@bs.deriving accessors]
type src =
  | SrcObject(MediaStream.t)
  | SrcUrl(string)
  | SrcElement(array((string, mimeType)));

[@react.component]
let make = (~autoPlay=false, ~controls=false, ~src=?) => {
  let videoRef = React.useRef(Js.Nullable.null);

  let _ =
    React.useEffect0(() => {
      let _ =
        videoRef
        ->React.Ref.current
        ->Js.Nullable.toOption
        ->Belt.Option.map(
            doto(Webapi.Dom.Element.setAttribute("muted", "true")),
          )
        ->Belt.Option.map(
            doto(Webapi.Dom.Element.setAttribute("playsinline", "true")),
          );
      None;
    });

  let _ =
    React.useEffect1(
      () => {
        let _ =
          videoRef
          ->React.Ref.current
          ->Js.Nullable.toOption
          ->Belt.Option.map(elem => {
              let videoElem = VideoElement.unsafeAsVideoElement(elem);
              VideoElement.(
                switch (src) {
                | Some(SrcObject(srcObject)) =>
                  let _ =
                    videoElem->setSrcObject(srcObject->Js.Undefined.return);
                  ();
                | _ => ()
                }
              );
            });
        None;
      },
      [|src|],
    );

  let _ =
    React.useEffect1(
      () => {
        let _ =
          videoRef
          ->React.Ref.current
          ->Js.Nullable.toOption
          ->Belt.Option.map(elem => {
              let videoElem = elem->VideoElement.unsafeAsVideoElement;
              let paused = videoElem->VideoElement.paused;
              let readyState = videoElem->VideoElement.readyState;

              switch (readyState) {
              | _ when !autoPlay && !paused =>
                let _ = videoElem->VideoElement.pause;
                ();
              | `HaveNothing => ()
              | _ when autoPlay && paused =>
                videoElem
                ->VideoElement.setMuted(Js.Undefined.return(true))
                ->VideoElement.play
              | _ => ()
              };
            });
        None;
      },
      [|autoPlay|],
    );

  let handleLoadedMetadata = _ev => {
    let _ =
      videoRef
      ->React.Ref.current
      ->Js.Nullable.toOption
      ->Belt.Option.map(elem =>
          if (autoPlay) {
            let _ =
              elem
              ->VideoElement.unsafeAsVideoElement
              ->VideoElement.setMuted(Js.Undefined.return(true))
              ->VideoElement.play;
            ();
          }
        );
    ();
  };

  let handleError = ev => {
    Js.log2("error", ev);
  };

  let children =
    switch (src) {
    | Some(SrcElement(ss)) =>
      ss
      ->Belt.Array.map(((src, type_)) =>
          <source type_={type_->mimeTypeToJs} src key=src />
        )
      ->ReasonReact.array
    | _ => React.null
    };

  ReactDOMRe.createElementVariadic(
    "video",
    ~props=
      ReactDOMRe.objToDOMProps({
        "autoPlay": autoPlay,
        "controls": controls,
        "loop": !controls,
        "preload": "auto",
        "onError": handleError,
        "onLoadedMetadata": handleLoadedMetadata,
        "className": cn(["w-full", "h-full", "object-cover"]),
        "ref": videoRef->ReactDOMRe.Ref.domRef,
        "muted": true,
        "src":
          switch (src) {
          | Some(SrcUrl(url)) => Some(url)
          | _ => None
          },
      }),
    [|children|],
  );
};
