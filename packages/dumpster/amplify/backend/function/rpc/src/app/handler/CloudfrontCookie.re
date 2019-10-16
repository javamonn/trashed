let handler =
  Express.PromiseMiddleware.from((_next, request, response) =>
    Express.Response.(
      |> status(StatusCode.Ok)
      |> sendString("Ok.")
    );
  );
