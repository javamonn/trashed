module Event = {
  [@bs.deriving abstract]
  type outputDetail = pri {outputFilePaths: array(string)};

  [@bs.deriving abstract]
  type outputGroupDetail =
    pri {
      [@bs.as "type"]
      type_: string,
      outputDetails: array(outputDetail),
    };

  [@bs.deriving abstract]
  type detail =
    pri {
      jobId: string,
      status: string,
      outputGroupDetails: array(outputGroupDetail),
    };
  [@bs.deriving abstract]
  type t =
    pri {
      source: string,
      detail,
    };
};

type context;
type cb = (Js.Nullable.t(Js.Promise.error), Js.Json.t) => unit;
type handler = (Event.t, context, cb) => Js.Promise.t(Js.Json.t);
