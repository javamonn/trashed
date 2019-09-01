open Lib_Utils;

type t;

type loadedMetadataEvent;

external unsafeAsVideoElement: Dom.element => t = "%identity";

[@bs.send] external play: t => unit = "play";

[@bs.set]
external setSrcObject: (t, Js.Undefined.t(Externals_MediaStream.t)) => unit =
  "srcObject";

let setSrcObject = doto1(setSrcObject);

[@bs.set] external setSrc: (t, Js.Undefined.t(string)) => unit = "src";
let setSrc = doto1(setSrc);

[@bs.set]
external setOnLoadedMetadata: (t, loadedMetadataEvent => unit) => unit =
  "onloadedmetadata";

include Webapi.Dom.Element.Impl({
  type nonrec t = t;
});

let removeAttribute = doto1(flip2(removeAttribute));
