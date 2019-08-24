module Params = {
  [@bs.deriving abstract]
  type t = {phoneNumber: string};
  let make = t;

  let decode =
    Utils.optionalDecode(json =>
      make(~phoneNumber=json |> Json.Decode.(field("phoneNumber", string)))
    );
};

let handler =
  Express.PromiseMiddleware.from((_next, request, response) =>
    switch (
      request
      |> Express.Request.bodyJSON
      |> Js.Option.andThen(Utils.wrapBs(Params.decode))
    ) {
    | Some(p) =>
      TwilioService.sendMessage(
        ~to_=Params.phoneNumberGet(p),
        ~body=Constants.Strings.phoneNumberRegistered,
      )
      |> Js.Promise.then_(_r =>
           Express.Response.(
             response |> status(StatusCode.Ok) |> sendString("Ok.")
           )
           |> Js.Promise.resolve
         )
      |> Js.Promise.catch(ex => {
           Js.log(ex);
           Express.Response.(
             response
             |> status(StatusCode.ServiceUnavailable)
             |> sendString("Unable to process request.")
           )
           |> Js.Promise.resolve;
         })
    | None =>
      Express.Response.(
        response
        |> status(StatusCode.BadRequest)
        |> sendString("Missing phoneNumber.")
        |> Js.Promise.resolve
      )
    }
  );
