let make = () =>
  Express.Middleware.from((_next, _request, response) => {
    Js.log("path not found middleware");
    Express.Response.(
      response
      |> status(StatusCode.NotFound)
      |> sendString("Request path not found.")
    );
  });
