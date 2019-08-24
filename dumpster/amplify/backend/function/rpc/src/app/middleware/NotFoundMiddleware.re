let make = () =>
  Express.Middleware.from((_next, _request, response) =>
    Express.Response.(
      response
      |> status(StatusCode.NotFound)
      |> sendString("Request path not found.")
    )
  );
