[@bs.val] [@bs.scope ("window", "navigator", "serviceWorker")]
external register: string => unit = "register";

[@bs.deriving abstract]
type options = {scope: string};

[@bs.val] [@bs.scope ("window", "navigator", "serviceWorker")]
external registerWithOptions: (string, options) => unit = "register";
