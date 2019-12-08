open Lib.Constants;

let _ =
  Webapi.Dom.(
    window
    |> Window.addEventListener("load", _ => {
         let _ =
           Externals.ServiceWorker.(
             registerWithOptions(Env.serviceWorkerUrl, options(~scope="/"))
           );
         let _ = Sentry.(init(config(~dsn=Env.sentryDsn)));
         ();
       })
  );

ReactDOMRe.renderToElementWithId(<App />, "app");
