open Externals;

Js.log("Hello, Service Worker!");

let _ = Workbox.Precaching.precacheAndRoute(Workbox.precacheManifest);
