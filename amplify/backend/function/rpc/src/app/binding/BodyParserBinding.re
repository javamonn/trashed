module Json = {
  type t;
  type boundMiddleware =
    (Express.Request.t, Express.Response.t, Express.Next.t) => Express.complete;

  [@bs.val] [@bs.module "body-parser"]
  external make: unit => boundMiddleware = "json";
};
