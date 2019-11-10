let make = () =>
  Express.Middleware.from((next, request, response) =>
    switch (request |> Express.Request.httpMethod) {
    | Express.Request.Options =>
      Express.Response.(
        response
        |> setHeader(
             "access-control-allow-origin",
             (request |> Express.Request.get("origin"))
             ->Belt.Option.getWithDefault("*"),
           )
        |> setHeader(
             "access-control-allow-headers",
             (
               request |> Express.Request.get("access-control-request-headers")
             )
             ->Belt.Option.getWithDefault("*"),
           )
        |> setHeader(
             "access-control-allow-methods",
             (request |> Express.Request.get("access-control-request-method"))
             ->Belt.Option.getWithDefault("*"),
           )
        |> setHeader("access-control-max-age", "86400")
        |> setHeader(
             "vary",
             "origin, access-control-request-headers, access-control-request-method",
           )
        |> setHeader("access-control-allow-credentials", "true")
        |> status(StatusCode.Ok)
        |> sendString("Ok.")
      )
    | _ =>
      Express.Response.(
        response
        |> setHeader(
             "access-control-allow-origin",
             (request |> Express.Request.get("origin"))
             ->Belt.Option.getWithDefault("*"),
           )
        |> setHeader("access-control-allow-credentials", "true")
        |> setHeader(
             "access-control-expose-headers",
             "cache-control, content-language, content-type, expires, last-modified, pragma, set-cookie",
           )
        |> setHeader("vary", "origin")
        |> next(Express.Next.middleware)
      )
    }
  );
