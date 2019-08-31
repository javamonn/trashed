open Externals;
open Lib.Styles;
open Lib.Utils;

%raw
"import './app.pcss'";

[@react.component]
let make = () => {
  let videoRef = React.useRef(Js.Nullable.null);
  let _ =
    React.useEffect0(() => {
      let _ =
        MediaDevices.MediaConstraints.(
          make(~audio=true, ~video=makeVideo(~facingMode="environment"), ())
        )
        |> Externals_MediaDevices.getUserMedia
        |> Js.Promise.then_(stream => {
             let _ =
               switch (videoRef |> React.Ref.current |> Js.Nullable.toOption) {
               | Some(elem) =>
                 let _ =
                   elem
                   *> Webapi.Dom.Element.setAttribute("muted", "true")
                   *> Webapi.Dom.Element.setAttribute("playsinline", "true");
                 let videoElem = VideoElement.unsafeAsVideoElement(elem);
                 let _ =
                   videoElem
                   *> VideoElement.setSrcObject(stream)
                   *> VideoElement.setOnLoadedMetadata(_ev => {
                        let _ = VideoElement.play(videoElem);
                        ();
                      });
                 ();
               | None => ()
               };

             Js.Promise.resolve();
           });
      None;
    });

  <video
    className={cn(["h-screen", "w-screen", "object-cover"])}
    ref={ReactDOMRe.Ref.domRef(videoRef)}
  />;
};
