type t;

type loadedMetadataEvent;

external unsafeAsVideoElement: Dom.element => t = "%identity";
[@bs.send] external play: t => unit = "";
[@bs.set]
external setSrcObject: (t, Externals_MediaStream.t) => unit = "srcObject";
[@bs.set]
external setOnLoadedMetadata: (t, loadedMetadataEvent => unit) => unit =
  "onloadedmetadata";
