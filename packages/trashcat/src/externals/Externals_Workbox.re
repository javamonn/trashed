module Precaching = {
  [@bs.deriving abstract]
  type precacheManifestItem = {
    revision: string,
    url: string,
  };

  [@bs.module "workbox-precaching/precacheAndRoute.mjs"]
  external precacheAndRoute: array(precacheManifestItem) => unit =
    "precacheAndRoute";
};

[@bs.scope "self"] [@bs.val]
external precacheManifest: array(Precaching.precacheManifestItem) =
  "__WB_MANIFEST";
