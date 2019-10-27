let make = () => {
  let middleware = BodyParserBinding.Json.make();
  Js.log("Make ParseBodyAsJsonMiddleware");
  Express.Middleware.from((next, req, res) => {
    Js.log("ParseBodyAsJsonMiddleware");
    middleware(req, res, next);
  });
};
