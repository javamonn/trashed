module Json = {
  type t;

  [@bs.send] [@bs.module "body-parser"]
  external make: unit => Express.Middleware.t = "json";
}
