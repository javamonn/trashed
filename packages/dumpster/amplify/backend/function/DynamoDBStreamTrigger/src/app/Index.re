open Externals;
open Lib;

let _ = AwsAmplify.(inst->configure(Constants.awsAmplifyConfig));

module Lambda = {
  [@bs.deriving abstract]
  type event = {
    [@bs.as "Records"]
    records: array(DynamoDBStreamRecord.t),
  };
  type context;
  type cb = (. Js.Nullable.t(Js.Promise.error), Js.Json.t) => unit;
  type handler = (event, context, cb) => Js.Promise.t(Js.Json.t);
};

let handler: Lambda.handler =
  (event, _context, _cb) => {
    Js.log2("event", Js.Json.stringifyAny(event));
    event
    ->Lambda.recordsGet
    ->Belt.Array.map(r =>
        switch (
          r->DynamoDBStreamRecord.eventNameGet,
          r->DynamoDBStreamRecord.tableGet,
        ) {
        | ("INSERT", Some(`VideoTable)) => Handler.onVideoCreate(r)
        | ("UPDATE", Some(`VideoTable)) => Handler.onVideoUpdate(r)
        | _ => Js.Promise.resolve(Belt.Result.Ok())
        }
      )
    ->Js.Promise.all
    |> Js.Promise.then_(r => {
         let eventResults =
           Belt.Array.zip(r, event->Lambda.recordsGet)
           ->Belt.Array.map(((result, record)) => {
               let (message, status) =
                 switch (result) {
                 | Belt.Result.Ok(_) => ("Ok.", 200)
                 | Belt.Result.Error(e) => (e, 500)
                 };
               Json.Encode.(
                 object_([
                   (
                     "eventId",
                     record->DynamoDBStreamRecord.eventIdGet->string,
                   ),
                   (
                     "eventName",
                     record->DynamoDBStreamRecord.eventNameGet->string,
                   ),
                   ("message", message->string),
                   ("status", status->int),
                 ])
               );
             });
         Json.Encode.(
           object_([
             ("status", int(200)),
             ("results", jsonArray(eventResults)),
           ])
         )
         ->Js.Promise.resolve;
       })
    |> Js.Promise.catch(ex => {
         Js.log(ex);
         Json.Encode.(object_([("status", int(500))]))->Js.Promise.resolve;
       });
  };
