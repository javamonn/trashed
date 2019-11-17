open Lib;

%raw
"import './app.pcss'";

let _ = AwsAmplify.(inst->configure(Constants.awsAmplifyConfig));

[@react.component]
let make = () => {
  let url = ReasonReactRouter.useUrl();

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
