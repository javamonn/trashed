open Externals;
open Lib.Styles;
open Lib.Utils;

[@bs.deriving accessors]
type src =
  | SrcObject(MediaStream.t)
  | SrcUrl(string);

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
                  videoElem
                  ->setSrcObject(srcObject->Js.Nullable.return)
                  ->setSrc(Js.Nullable.null)
                | Some(SrcUrl(src)) =>
                  videoElem
                  ->setSrc(src->Js.Nullable.return)
                  ->setSrcObject(Js.Nullable.null)
                | None =>
                  videoElem
                  ->setSrc(Js.Nullable.null)
                  ->setSrcObject(Js.Nullable.null)
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
          ->Belt.Option.map(elem =>
              if (autoPlay) {
                let videoElem = elem->VideoElement.unsafeAsVideoElement;
                let _ =
                  videoElem->VideoElement.setOnLoadedMetadata(_ev => {
                    let _ = VideoElement.play(videoElem);
                    ();
                  });
                ();
              }
            );
        None;
      },
      [|autoPlay|],
    );

  <video
    autoPlay
    controls
    className={cn(["w-full", "h-full", "object-cover"])}
    ref={videoRef->ReactDOMRe.Ref.domRef}
  />;
};
