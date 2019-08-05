let make = () => (request, response, next) => {
  let _ =
    Express.Response.(
      response
        |> header(Header.AccessControlAllowOrigin, "*")
        |> header(Header.AccessControlAllowHeaders, "Origin, X-Requested-With, Content-Type, Accept")
    );
  next();
};
