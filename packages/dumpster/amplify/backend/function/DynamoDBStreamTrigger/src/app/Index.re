module Lambda = {
  [@bs.deriving abstract]
  type event = {
    [@bs.as "Records"]
    records: array(DynamoDBStreamRecord.t),
  };
  type context;
  type cb = (Js.Nullable.t(Js.Promise.error), event) => unit;
  type handler = (event, context, cb) => unit;
};

let handleMediaConvertComplete = r => Js.Promise.resolve();

let handler: Lambda.handler =
  (event, _context, cb) => {
    let _ =
      event
      ->Lambda.recordsGet
      ->Belt.Array.map(r =>
          switch (
            r->DynamoDBStreamRecord.eventNameGet,
            r->DynamoDBStreamRecord.tableGet,
          ) {
          | ("INSERT", Some(`VideoTable)) => HandleMediaConvertSubmit.handle(r)
          | ("UPDATE", Some(`VideoTable)) => handleMediaConvertComplete(r)
          | _ => Js.Promise.resolve()
          }
        )
      ->Js.Promise.all
      |> Js.Promise.then_(_ => {
           let _ = cb(Js.Nullable.null, event);
           Js.Promise.resolve();
         })
      |> Js.Promise.catch(ex => {
           Js.log(ex);
           let _ = cb(ex |> Js.Nullable.return, event);
           Js.Promise.resolve();
         });
    ();
  };
