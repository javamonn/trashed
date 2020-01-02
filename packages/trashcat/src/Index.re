open Lib.Constants;

let _ = Service.RootHeightManager.make();

let _ =
  Webapi.Dom.(
    window
    |> Window.addEventListener("load", _ => {
         let _ =
           Externals.ServiceWorker.(
             registerWithOptions(Env.serviceWorkerUrl, options(~scope="/"))
           );
         let _ =
           if (Env.nodeEnv !== "development") {
             Sentry.(init(config(~dsn=Env.sentryDsn)));
           };
         ();
       })
  );

ReactDOMRe.renderToElementWithId(<App />, "app");
