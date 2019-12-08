open Externals;

let _ = Workbox.(precacheManifest->Precaching.precacheAndRoute);
let _ =
  Workbox.(
    Routing.registerNavigationRoute(
      Precaching.getCacheKeyForURL("/index.html"),
      Routing.config(~whitelist=[|[%bs.re "/.*/"]|]),
    )
  );

let _ =
  Workbox.(
    Routing.registerRoute(
      [%bs.re "/\.(?:png|gif|jpg|jpeg|webp|svg)$/"],
      Strategies.strategyConfig(
        ~cacheName="images",
        ~plugins=[|
          Expiration.pluginConfig(
            ~maxEntries=60,
            ~maxAgeSeconds=10 * 24 * 60 * 60 /*** 10 days **/
          )
          ->Expiration.plugin,
        |],
      )
      ->Strategies.cacheFirst,
    )
  );
