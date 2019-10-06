open Lib;

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

let _ = AwsAmplify.(inst->configure(Constants.awsAmplifyConfig));

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
      |> Js.Promise.then_(result => {
           let (message, status) =
             switch (result) {
             | Belt.Result.Ok(_) => ("Ok.", 200)
             | Belt.Result.Error(e) => (e, 500)
             };
           let _ =
             cb(
               Js.Nullable.null,
               Json.Encode.(
                 object_([
                   ("status", status->int),
                   ("message", message->string),
                   (
                     "event",
                     object_([
                       ("jobId", string(jobId)),
                       ("jobStatus", string(jobStatus)),
                     ]),
                   ),
                 ])
               ),
             );
           Js.Promise.resolve();
         });
    ();
  };
