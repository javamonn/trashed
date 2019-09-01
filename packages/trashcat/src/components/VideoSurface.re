open Externals;
open Lib.Styles;
open Lib.Utils;

[@bs.deriving accessors]
type src =
  | SrcObject(MediaStream.t)
  | SrcUrl(string)
  | SrcElement(array((string, string)));

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
                  ->setSrcObject(srcObject->Js.Undefined.return)
                  ->setSrc(Js.undefined)
                  ->removeAttribute("src")
                | Some(SrcUrl(src)) =>
                  videoElem
                  ->setSrc(src->Js.Undefined.return)
                  ->setSrcObject(Js.undefined)
                  ->removeAttribute("srcObject")
                | _ =>
                  videoElem
                  ->setSrc(Js.undefined)
                  ->setSrcObject(Js.undefined)
                  ->removeAttribute("src")
                  ->removeAttribute("srcObject")
                }
              );
            });
        None;
      },
      [|src|],
    );

  let handleLoadedMetadata = _ev => {
    let _ =
      videoRef
      ->React.Ref.current
      ->Js.Nullable.toOption
      ->Belt.Option.map(elem =>
          if (autoPlay) {
            let _ = elem->VideoElement.unsafeAsVideoElement->VideoElement.play;
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
      ->Belt.Array.map(((src, type_)) => <source type_ src />)
      ->ReasonReact.array
    | _ => React.null
    };

  <video
    autoPlay
    controls
    onError=handleError
    onLoadedMetadata=handleLoadedMetadata
    className={cn(["w-full", "h-full", "object-cover"])}
    ref={videoRef->ReactDOMRe.Ref.domRef}>
    children
  </video>;
};
