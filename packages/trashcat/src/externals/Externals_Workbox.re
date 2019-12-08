module Precaching = {
  [@bs.deriving abstract]
  type precacheManifestItem = {
    revision: string,
    url: string,
  };

  [@bs.module "workbox-precaching/precacheAndRoute.mjs"]
  external precacheAndRoute: array(precacheManifestItem) => unit =
    "precacheAndRoute";

  [@bs.module "workbox-precaching/getCacheKeyForURL.mjs"]
  external getCacheKeyForURL: string => string = "getCacheKeyForURL";
};

module Routing = {
  [@bs.module "workbox 
};

module Expiration = {
  type t;

  [@bs.deriving abstract]
  type pluginConfig = {
    maxEntries: int,
    maxAgeSeconds: int,
  };

  [@bs.module "workbox-expiration/Plugin.mjs"] [@bs.new]
  external plugin: pluginConfig => t = "Plugin";
};

module Strategies = {
  type t;

  [@bs.deriving abstract]
  type strategyConfig = {
    cacheName: string,
    plugins: array(Expiration.t),
  };

  [@bs.module "workbox-strategies/CacheFirst.mjs"] [@bs.new]
  external cacheFirst: strategyConfig => t = "CacheFirst";
};

module Routing = {
  [@bs.module "workbox-routing/registerRoute.mjs"]
  external registerRoute: (Js.Re.t, Strategies.t) => unit = "registerRoute";
};

[@bs.scope "self"] [@bs.val]
external precacheManifest: array(Precaching.precacheManifestItem) =
  "__WB_MANIFEST";
