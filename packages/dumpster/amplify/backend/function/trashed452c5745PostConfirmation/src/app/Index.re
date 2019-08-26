[@bs.deriving abstract]
type request = pri {userAttributes: Js.Dict.t(string)};

type response;

[@bs.deriving abstract]
type event =
  pri {
    mutable request,
    response,
  };

type context;

type cb = (Js.Nullable.t(Js.Promise.error), event) => unit;

type handler = (event, context, cb) => unit;

let handler: handler =
  (event, _context, cb) => {
    switch (event->requestGet->userAttributesGet->Js.Dict.get("phone_number")) {
    | Some(phoneNumber) =>
      let _ =
        TwilioService.sendMessage(
          ~to_=phoneNumber,
          ~body=Constants.Strings.phoneNumberRegistered,
        )
        |> Js.Promise.then_(_r => {
             let _ = cb(Js.Nullable.null, event);
             Js.Promise.resolve();
           })
        |> Js.Promise.catch(ex => {
             Js.log(ex);
             let _ = cb(ex |> Js.Nullable.return, event);
             Js.Promise.resolve();
           });
      ();
    | None =>
      Js.log("Expected phone_number userAttribute, but found none.");
      let _ = cb(Js.Nullable.null, event);
      ();
    };
  };
