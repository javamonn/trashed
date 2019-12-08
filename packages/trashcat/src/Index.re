open Lib.Constants;

let _ =
  Webapi.Dom.(
    window
    |> Window.addEventListener("load", _ => {
         Js.log("debug");
         Externals.ServiceWorker.(
           registerWithOptions(Env.serviceWorkerUrl, options(~scope="/"))
         );
       })
  );

ReactDOMRe.renderToElementWithId(<App />, "app");
