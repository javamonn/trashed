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

  /**
let _ =
  Workbox.Routing.registerRoute(
    [%bs.re "/(https|http):\/\/trashcat-cdn\.trashed\.today\/(.*)$/"],
    `Handler(
      ctx => {
        switch (Workbox.Routing.(ctx->request)) {
        | Some(request) =>
          let entries = [%raw "function(c) { return Array.from(c.entries()) }"];
          let _ =
            Js.log2(
              "headers",
              request |> Fetch.Request.headers |> entries,
            );
          request->Fetch.fetchWithRequest;
        | None => Workbox.Routing.makeResponseError()->Js.Promise.resolve
        }
      },
    ),
  );
**/

