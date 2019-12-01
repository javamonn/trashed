open Externals;
open Lib.Constants;

let _ =
  Webapi.Dom.(
    window
    |> Window.addEventListener("load", _ =>
         Externals.ServiceWorker.register(Env.serviceWorkerUrl)
       )
  );

ReactDOMRe.renderToElementWithId(<App />, "app");
