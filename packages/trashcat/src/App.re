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
  | ["item", "new"] =>
    <Providers.Apollo> <Screen.NewItem /> </Providers.Apollo>
  | ["item"] => <Providers.Apollo> <Screen.ListItems /> </Providers.Apollo>
  | _ => ReasonReact.string("Nothing here!")
  };
};
