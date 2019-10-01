open Lib;

module Config = {
  type t;

  [@bs.send] external update: (t, {. "region": string}) => unit = "update";
  [@bs.set]
  external mediaconvert: (t, {. "endpoint": string}) => unit = "mediaconvert";
};

module MediaConvert = {
  type t;
  type jobResponse;

  module Job = {
    [@bs.deriving abstract]
    type job = {
      [@bs.as "Id"]
      id: string,
    };
    [@bs.deriving abstract]
    type t = {
      [@bs.as "Job"]
      job,
    };
  };

  [@bs.new] [@bs.module "aws-sdk"]
  external make:
    {
      .
      "apiVersion": string,
      "region": string,
      "endpoint": string,
    } =>
    t =
    "MediaConvert";

  [@bs.send] external createJob: (t, Js.t({..})) => jobResponse = "createJob";

  [@bs.send] external promise: jobResponse => Js.Promise.t(Job.t) = "promise";

  let service =
    make({
      "apiVersion": "2017-08-29",
      "region": Constants.Env.region,
      "endpoint": Constants.Env.mediaConvertEndpoint,
    });
};

[@bs.deriving abstract]
type t = {config: Config.t};

[@bs.module] external inst: t = "aws-sdk";

/** Global configuration **/
let _ = inst->configGet->Config.update({"region": Constants.Env.region});
