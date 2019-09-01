open Lib_Utils;

type t;

type loadedMetadataEvent;

external unsafeAsVideoElement: Dom.element => t = "%identity";

[@bs.send] external play: t => unit = "play";

[@bs.set]
external setSrcObject: (t, Js.Nullable.t(Externals_MediaStream.t)) => unit =
  "srcObject";

let setSrcObject = doto1(setSrcObject);

[@bs.set] external setSrc: (t, Js.Nullable.t(string)) => unit = "src";
let setSrc = doto1(setSrc);

[@bs.set]
external setOnLoadedMetadata: (t, loadedMetadataEvent => unit) => unit =
  "onloadedmetadata";
