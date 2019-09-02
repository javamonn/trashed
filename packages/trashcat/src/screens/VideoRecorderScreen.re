open Lib;
open Externals;

[@react.component]
let make = () => {
  let handleFile = f => {
    Js.log2("file", f);
  };

  switch (Constants.browser->Bowser.getBrowserName) {
  | Some(`Safari) =>
    <VideoRecorder.FileInput mimeType="video/webm" onFile=handleFile />
  | _ =>
    <VideoRecorder.MediaRecorder mimeType="video/webm" onFile=handleFile />
  };
};
