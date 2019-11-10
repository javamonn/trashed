let make = () => {
  let middleware = AWSServerlessExpressMiddlewareBinding.EventContext.make();

  Express.Middleware.from((next, req, res) => {
    Js.log("AWSServerlessExpressMiddleware");
    middleware(req, res, next);
  });
};
