open Externals;
open Lib.Styles;

[%%raw "import './app.pcss'"];

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
                 let videoElem = VideoElement.unsafeAsVideoElement(elem);
                 let _ = VideoElement.setSrcObject(videoElem, stream);
                 let _ =
                   VideoElement.setOnLoadedMetadata(
                     videoElem,
                     _ev => {
                       let _ = VideoElement.play(videoElem);
                       ();
                     },
                   );
                 ();
               | None => ()
               };

             Js.Promise.resolve();
           });
      None;
    });

  <video
    className={cn(["h-screen", "w-screen"])}
    ref={ReactDOMRe.Ref.domRef(videoRef)}
  />;
};
