let make = () =>
  Express.Middleware.from((next, _request, response) => {
    Js.log("CORSMiddleware");
    response
    |> Express.Response.setHeader("access-control-allow-origin", "*")
    |> Express.Response.setHeader(
         "access-control-allow-headers",
         "Origin, X-Requested-With, Content-Type, Accept",
       )
    |> next(Express.Next.middleware);
  });
