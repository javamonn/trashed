type t;

module Player = {
  [@bs.deriving abstract]
  type options = {debug: bool};
  type t = Externals_VideoElement.t;
};

module Loader = {
  type t;
};

[@bs.module "ogv"] external loader: Loader.t = "OGVLoader";
[@bs.set] external setBase: (Loader.t, string) => unit = "base";
let setBase = loader->setBase;

[@bs.new] [@bs.module "ogv"]
external makePlayer: Player.options => Player.t = "OGVPlayer";
