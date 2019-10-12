open Lib;
open Externals;

let _ = AwsAmplify.(inst->configure(Constants.awsAmplifyConfig));

let handler: Lambda.handler =
  (event, _context, cb) => {
    let jobStatus = event->Lambda.Event.detailGet->Lambda.Event.statusGet;
    let p =
      switch (jobStatus) {
      | "COMPLETE" =>
        Handlers_OnComplete.handle(event->Lambda.Event.detailGet)
      | _ => Belt.Result.Ok()->Js.Promise.resolve
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
                       (
                         "jobId",
                         event
                         ->Lambda.Event.detailGet
                         ->Lambda.Event.jobIdGet
                         ->string,
                       ),
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
