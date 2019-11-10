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

[@bs.val] [@bs.scope ("window", "navigator", "mediaDevices")]
external getUserMedia:
  MediaConstraints.t => Js.Promise.t(Externals_MediaStream.t) =
  "getUserMedia";
