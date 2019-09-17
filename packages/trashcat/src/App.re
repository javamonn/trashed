open Externals;

%raw
"import './app.pcss'";

let _ = Amplify.(inst->configure(config));

[@react.component]
let make = () => {
  let url = ReasonReactRouter.useUrl();

  switch (url.path) {
  | ["item", "new"] =>
    <Providers.Apollo> <Screen.NewItem /> </Providers.Apollo>
  | ["item"] => <Providers.Apollo> <Screen.ListItems /> </Providers.Apollo>
  | ["item", itemId] =>
    <Providers.Apollo> <Screen.Item itemId /> </Providers.Apollo>
  | _ => ReasonReact.string("Nothing here!")
  };
};
