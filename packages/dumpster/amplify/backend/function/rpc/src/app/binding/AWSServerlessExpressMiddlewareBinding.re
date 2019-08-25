module EventContext = {
  type t;
  type boundMiddleware =
    (Express.Request.t, Express.Response.t, Express.Next.t) => Express.complete;

  [@bs.val] [@bs.module "aws-serverless-express/middleware"]
  external make: unit => boundMiddleware = "eventContext";
};
