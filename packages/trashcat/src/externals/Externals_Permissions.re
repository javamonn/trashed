[@bs.deriving jsConverter]
type permission = [
  | [@bs.as "camera"] `Camera
  | [@bs.as "geolocation"] `Geolocation
  | [@bs.as "notifications"] `Notifications
  | [@bs.as "push"] `Push
  | [@bs.as "microphone"] `Microphone
];

module Status = {
  [@bs.deriving jsConverter]
  type state = [
    | [@bs.as "granted"] `Granted
    | [@bs.as "denied"] `Denied
    | [@bs.as "prompt"] `Prompt
  ];

  let state_encode = s => s->stateToJs->Js.Json.string;
  let state_decode = s =>
    switch (s->Js.Json.decodeString->Belt.Option.flatMap(stateFromJs)) {
    | Some(s) => Belt.Result.Ok(s)
    | None => Decco.error("Unable to decode state", Js.Json.null)
    };

  [@decco]
  [@bs.deriving accessors]
  type t = {state};

  let encode = t_encode;
  let decode = t_decode;
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
  external query: (t, Descriptor.js) => Js.Promise.t(Status.t) = "query";

  let query = descriptor =>
    api->Belt.Option.map(api => api->query(descriptor));
};

let query = d =>
  d
  ->Descriptor.toJs
  ->External.query
  ->Belt.Option.map(p =>
      p |> Js.Promise.then_(result => result->Js.Promise.resolve)
    );
