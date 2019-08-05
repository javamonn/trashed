module EventContext = {
  type t;

  [@bs.send] [@bs.module "aws-serverless-express/middleware"]
  external make: unit => Express.Middleware.t = "eventContext";
}
