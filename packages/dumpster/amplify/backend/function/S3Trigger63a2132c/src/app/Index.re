module Lambda = {
  [@bs.deriving abstract]
  type event = {
    [@bs.as "Records"]
    records: array(S3EventRecord.t),
  };
  type context;
  type cb = (Js.Nullable.t(Js.Promise.error), event) => unit;
  type handler = (event, context, cb) => unit;
};

let handler: Lambda.handler =
  (event, _context, cb) => {
    let _ =
      event
      ->Lambda.recordsGet
      ->Belt.Array.map(r =>
          switch (
            S3EventRecord.(r->eventNameGet),
            S3EventRecord.(r->s3Get->bucketGet->nameGet),
            S3EventRecord.(r->s3Get->object_Get->keyGet->primaryDir),
          ) {
          | ("s3ObjectCreated:*", "trashcat", "public/item-video-uploads") =>
            ItemVideoUpload.handle(r)
          | ("s3ObjectRemoved:*", "trashcat", _) => Js.Promise.resolve()
          | (eventType, bucket, topDir) =>
            Js.log4("Unhandled event: ", eventType, bucket, topDir);
            Js.Promise.resolve();
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
