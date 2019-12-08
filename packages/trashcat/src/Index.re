open Lib.Constants;

let _ =
  Webapi.Dom.(
    window
    |> Window.addEventListener("load", _ =>
         Externals.ServiceWorker.(
           registerWithOptions(Env.serviceWorkerUrl, options(~scope="/"))
         )
       )
  );

ReactDOMRe.renderToElementWithId(<App />, "app");
