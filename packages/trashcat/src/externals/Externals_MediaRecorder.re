type t;

[@bs.deriving abstract]
type options = {mimeType: string};

[@bs.new]
external make: (Externals_MediaStream.t, Js.Nullable.t(options)) => t =
  "MediaRecorder";

[@bs.send] external start: t => unit = "start";
[@bs.send] external stop: t => unit = "stop";

[@bs.deriving jsConverter]
type state = [
  | [@bs.as "recording"] `Recording
  | [@bs.as "paused"] `Paused
  | [@bs.as "inactive"] `Inactive
];
[@bs.get] external state: t => string = "state";
let state = t => t->state->stateFromJs;

include Webapi.Dom.EventTarget.Impl({
  type nonrec t = t;
});

module Event = {
  [@bs.deriving abstract]
  type dataAvailableEvent = pri {data: Externals_File.t};
  external toDataAvailable: Dom.event => dataAvailableEvent = "%identity";

  type t =
    | Start
    | Stop
    | DataAvailable
    | Error;
};
