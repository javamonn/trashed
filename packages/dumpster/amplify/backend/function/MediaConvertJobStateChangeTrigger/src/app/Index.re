module Event = {
  [@bs.deriving abstract]
  type detail =
    pri {
      jobId: string,
      status: string,
    };
  [@bs.deriving abstract]
  type t =
    pri {
      source: string,
      detail,
    };
};

module Lambda = {
  type context;
  type cb = (Js.Nullable.t(Js.Promise.error), Js.Json.t) => unit;
  type handler = (Event.t, context, cb) => unit;
};

let handler: Lambda.handler =
  (event, _context, cb) => {
    let jobId = event->Event.detailGet->Event.jobIdGet;
    let jobStatus = event->Event.detailGet->Event.statusGet;
    let p =
      switch (jobStatus, jobId) {
      | ("COMPLETE", jobId) => Handlers_OnComplete.handle(jobId)
      };
    let _ =
      p
      |> Js.Promise.then_(_result => {
           let _ =
             cb(
               Js.Nullable.null,
               Json.Encode.(
                 object_([
                   ("jobId", string(jobId)),
                   ("jobStatus", string(jobStatus)),
                 ])
               ),
             );
           Js.Promise.resolve();
         });
    ();
  };
