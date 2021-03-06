open Externals;

let _ = Workbox.(precacheManifest->Precaching.precacheAndRoute);
let _ =
  Workbox.(
    "/index.html"
    ->Precaching.getCacheKeyForURL
    ->Routing.registerNavigationRoute
  );

let _ =
  Workbox.(
    Routing.registerRoute(
      [%bs.re "/\.(?:png|gif|jpg|jpeg|webp|svg)$/"],
      `Strategy(
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
      ),
    )
  );

let _ =
  Workbox.(
    Routing.registerRoute(
      [%bs.re "/https:\/\/api.mapbox.com\/styles\/(.*)$/"],
      `Strategy(
        Strategies.strategyConfig(
          ~cacheName="mapbox-tiles",
          ~plugins=[|
            Expiration.pluginConfig(
              ~maxEntries=60,
              ~maxAgeSeconds=10 * 24 * 60 * 60 /*** 10 days **/
            )
            ->Expiration.plugin,
          |],
        )
        ->Strategies.cacheFirst,
      ),
    )
  );
