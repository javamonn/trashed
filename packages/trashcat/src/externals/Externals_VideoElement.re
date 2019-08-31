open Lib_Utils;

type t;

type loadedMetadataEvent;

external unsafeAsVideoElement: Dom.element => t = "%identity";
[@bs.send] external play: t => unit = "";
[@bs.set]
external setSrcObject: (t, Externals_MediaStream.t) => unit = "srcObject";
let setSrcObject = flip(setSrcObject);

[@bs.set]
external setOnLoadedMetadata: (t, loadedMetadataEvent => unit) => unit =
  "onloadedmetadata";
let setOnLoadedMetadata = flip(setOnLoadedMetadata);
