[@bs.deriving jsConverter]
type permission = [
  | [@bs.as "camera"] `Camera
  | [@bs.as "geolocation"] `Geolocation
  | [@bs.as "notifications"] `Notifications
  | [@bs.as "push"] `Push
  | [@bs.as "microphone"] `Microphone
];

[@bs.deriving jsConverter]
type status = [
  | [@bs.as "granted"] `Granted
  | [@bs.as "denied"] `Denied
  | [@bs.as "prompt"] `Prompt
];

module Status = {
  [@bs.deriving abstract]
  type t = {state: status};
  let make = t;

  type js = {. "state": string};

  let fromJs = inst =>
    inst##state->statusFromJs->Belt.Option.map(state => make(~state));
};

module Descriptor = {
  [@bs.deriving abstract]
  type t = {name: permission};
  let make = t;

  type js = {. "name": string};

  let fromJs = inst =>
    inst##name->permissionFromJs->Belt.Option.map(name => make(~name));

  let toJs = inst => {"name": inst->nameGet->permissionToJs};
};

module External = {
  type t;

  [@bs.val] [@bs.scope ("window", "navigator")]
  external api: option(t) = "permissions";

  [@bs.send]
  external query: (t, Descriptor.js) => Js.Promise.t(Status.js) = "query";

  let query = descriptor =>
    api->Belt.Option.map(api => api->query(descriptor));
};

let query = d =>
  d
  ->Descriptor.toJs
  ->External.query
  ->Belt.Option.map(p =>
      p
      |> Js.Promise.then_(result => result->Status.fromJs->Js.Promise.resolve)
    );
