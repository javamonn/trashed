module Config = {
  type t;

  [@bs.send] external update: (t, {. "region": string}) => unit = "update";
  [@bs.set]
  external mediaconvert: (t, {. "endpoint": string}) => unit = "mediaconvert";
};

module MediaConvert = {
  type t;
  type jobResponse;
  type job;

  [@bs.new] [@bs.module "aws-sdk"]
  external make: {. "apiVersion": string} => t = "MediaConvert";

  [@bs.send] external createJob: (t, Js.t({..})) => jobResponse = "createJob";

  [@bs.send] external promise: t => Js.Promise.t(job) = "promise";
};

[@bs.deriving abstract]
type t = {config: Config.t};

[@bs.module "aws-sdk"] external inst: t = "default";
