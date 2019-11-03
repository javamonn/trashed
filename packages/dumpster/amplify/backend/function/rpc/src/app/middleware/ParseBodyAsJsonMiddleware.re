let make = () => {
  let middleware = BodyParserBinding.Json.make();
  Express.Middleware.from((next, req, res) => {
    Js.log("ParseBodyAsJsonMiddleware");
    middleware(req, res, next);
  });
};
