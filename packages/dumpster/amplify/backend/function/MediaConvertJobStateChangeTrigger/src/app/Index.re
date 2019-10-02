module Lambda = {
  [@bs.deriving abstract]
  type event = {
    [@bs.as "Records"]
    records: array(DynamoDBStreamRecord.t),
  };
  type context;
  type cb = (Js.Nullable.t(Js.Promise.error), Js.Json.t) => unit;
  type handler = (event, context, cb) => unit;
};

let handler: Lambda.handler =
  (event, _context, cb) => {
    Js.log2("event", event)
  };
