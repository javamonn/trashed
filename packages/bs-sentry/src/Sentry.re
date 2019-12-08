[@bs.deriving abstract]
type config = {dsn: string};

[@bs.module "@sentry/browser"] external init: config => unit = "init";
