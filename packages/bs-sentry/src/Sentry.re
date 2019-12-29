[@bs.deriving abstract]
type config = {dsn: string};

[@bs.module "@sentry/browser"] external init: config => unit = "init";

[@bs.module "@sentry/browser"]
external captureException: Js.Exn.t => unit = "captureException";
