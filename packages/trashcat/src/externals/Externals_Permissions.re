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

[@bs.val] [@bs.scope ("window", "navigator", "permissions")]
external query: Descriptor.js => Js.Promise.t(Status.js) = "query";

let query = d =>
  d->Descriptor.toJs->query
  |> Js.Promise.then_(res => res->Status.fromJs->Js.Promise.resolve);
