open Lib;

%raw
"import './app.pcss'";

let _ = AwsAmplify.(inst->configure(Constants.awsAmplifyConfig));
let _ = Webapi.Dom.(Window.scrollTo(0., 1., window));

[@react.component]
let make = () => {
  let url = ReasonReactRouter.useUrl();

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

  switch (url.path) {
  | [] => <Screen.Landing />
  | ["coming-soon"] => <Screen.ComingSoon />
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
      <Providers.Apollo> <Screen.Item tab url /> </Providers.Apollo>
    | None => <Redirect to_="/item/feed" />
    };
  | _ => ReasonReact.string("Nothing here!")
  };
};
