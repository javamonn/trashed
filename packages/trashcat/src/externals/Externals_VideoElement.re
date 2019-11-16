open Lib_Utils;

type t;

type loadedMetadataEvent;

external unsafeAsVideoElement: Dom.element => t = "%identity";

[@bs.send] external play: t => unit = "play";

[@bs.send] external pause: t => unit = "pause";

[@bs.get]
external readyState:
  t =>
  [@bs.int] [
    | [@bs.as 0] `HaveNothing
    | [@bs.as 1] `HaveMetadata
    | [@bs.as 2] `HaveCurrentData
    | [@bs.as 3] `HaveFutureData
    | [@bs.as 4] `HaveEnoughData
  ] =
  "readyState";

[@bs.get] external paused: t => bool = "paused";

[@bs.set]
external setSrcObject: (t, Js.Undefined.t(Externals_MediaStream.t)) => unit =
  "srcObject";

let setSrcObject = doto1(setSrcObject);

[@bs.set] external setSrc: (t, Js.Undefined.t(string)) => unit = "src";
let setSrc = doto1(setSrc);

[@bs.set] external setMuted: (t, Js.Undefined.t(bool)) => unit = "muted";
let setMuted = doto1(setMuted);

[@bs.set]
external setOnLoadedMetadata: (t, loadedMetadataEvent => unit) => unit =
  "onloadedmetadata";

include Webapi.Dom.Element.Impl({
  type nonrec t = t;
});
include Webapi.Dom.EventTarget.Impl({
  type nonrec t = t;
});

let removeAttribute = doto1(flip2(removeAttribute));
let hasAttribute = flip2(hasAttribute);
