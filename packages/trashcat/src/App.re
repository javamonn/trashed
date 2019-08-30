module MediaConstraints = {
  /**
  [@bs.deriving abstract]
  type facingMode = {exact: string};
  let makeFacingMode = facingMode;
  **/

  [@bs.deriving abstract]
  type video = {facingMode: string};
  let makeVideo = video;

  [@bs.deriving abstract]
  type t = {
    audio: bool,
    [@bs.optional]
    video,
  };
  let make = t;
};

type mediaStream;

[@bs.val] [@bs.scope ("window", "navigator", "mediaDevices")]
external getUserMedia: MediaConstraints.t => Js.Promise.t(mediaStream) = "";

[@react.component]
let make = () => {
  let _ =
    React.useEffect0(() => {
      let _ =
        MediaConstraints.(
          make(~audio=true, ~video=makeVideo(~facingMode="environment"), ())
        )
        |> getUserMedia
        |> Js.Promise.then_(stream => {
             Js.log2("stream", stream);
             Js.Promise.resolve();
           });
      None;
    });

  <div> {ReasonReact.string("Trashed")} </div>;
};
