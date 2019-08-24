type client;

/** accountSid, authToken */
[@bs.module]
external make: (string, string) => client = "twilio";

/** messages resource */
module Messages = {
  type messagesClient;

  [@bs.get] external resource: client => messagesClient = "messages";

  [@bs.deriving abstract]
  type createParams = {
    from: string,
    body: string,
    [@bs.as "to"]
    to_: string,
  };

  [@bs.send]
  external create: (messagesClient, createParams) => Js.Promise.t(Js.Json.t) =
    "create";
};
