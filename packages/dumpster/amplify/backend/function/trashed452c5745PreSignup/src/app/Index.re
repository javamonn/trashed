[@bs.deriving abstract]
type validationData =
  pri {
    [@bs.as "Name"]
    name: string,
    [@bs.as "Value"]
    value: string,
  };

[@bs.deriving abstract]
type request =
  pri {
    userAttributes: Js.Dict.t(string),
    validationData: array(validationData),
  };

[@bs.deriving abstract]
type response =
  pri {
    mutable autoConfirmUser: bool,
    mutable autoVerifyPhone: bool,
    mutable autoVerifyEmail: bool,
  };

[@bs.deriving abstract]
type event =
  pri {
    mutable request,
    mutable response,
  };

type context;

type cb = (Js.Nullable.t(Js.Exn.t), event) => unit;

type handler = (event, context, cb) => unit;

let handler: handler = (event, _context, cb) => {
  let _ = event->responseGet->(autoConfirmUserSet(true));
  let _ = event->responseGet->(autoVerifyPhoneSet(true));
  cb(Js.Nullable.null, event);
};
