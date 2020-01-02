open Lib;

%raw
"import './app.pcss'";

let _ = AwsAmplify.(inst->configure(Constants.awsAmplifyConfig));

[@react.component]
let make = () => {
  let url = ReasonReactRouter.useUrl();
  let (geolocationPermission, _, _) = Service.Permission.Geolocation.use();
  let (cameraPermission, _, _) = Service.Permission.Camera.use();

  let _ =
    React.useEffect0(() => {
      let disableContextMenu = ev => {
        let _ = Webapi.Dom.Event.preventDefault(ev);
        ();
      };
      let _ =
        Webapi.Dom.Document.addEventListener(
          "contextmenu",
          disableContextMenu,
          Webapi.Dom.document,
        );
      Some(
        () =>
          Webapi.Dom.Document.removeEventListener(
            "contextmenu",
            disableContextMenu,
            Webapi.Dom.document,
          ),
      );
    });

  let _ =
    React.useEffect1(
      () => {
        let shouldInitialize =
          switch (url.path) {
          | ["item", ..._] => true
          | _ => false
          };
        let _ =
          switch (geolocationPermission) {
          | Unknown when shouldInitialize =>
            Service.Permission.Geolocation.initialize()
          | _ => ()
          };
        let _ =
          switch (cameraPermission) {
          | Unknown when shouldInitialize =>
            Service.Permission.Camera.initialize()
          | _ => ()
          };
        None;
      },
      [|url.path |> Array.of_list |> Js.Array.joinWith("/")|],
    );

  switch (url.path) {
  | [] => <ErrorBoundary> <Screen.Landing /> </ErrorBoundary>
  | ["coming-soon"] => <ErrorBoundary> <Screen.ComingSoon /> </ErrorBoundary>
  | ["item", ...rest] =>
    let tab =
      switch (rest) {
      | ["new"] => Some(Screen.Item.Tab.New)
      | ["feed"] => Some(Screen.Item.Tab.Feed)
      | ["search"] => Some(Screen.Item.Tab.Search)
      | _ => None
      };

    switch (tab) {
    | Some(tab) =>
      <ErrorBoundary>
        <Providers.Apollo> <Screen.Item tab url /> </Providers.Apollo>
      </ErrorBoundary>
    | None => <Redirect to_="/item/feed" />
    };
  | _ => ReasonReact.string("Nothing here!")
  };
};
