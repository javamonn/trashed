let make = () => {
  let middleware = BodyParserBinding.Json.make();

  Express.Middleware.from((next, req, res) => middleware(req, res, next));
};
