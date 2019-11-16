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
  | ["item"] =>
    let params = Webapi.Url.URLSearchParams.make(url.search);
    <Providers.Apollo>
      <Screen.ListItems
        itemId=?{Webapi.Url.URLSearchParams.get("itemId", params)}
        nextToken=?{Webapi.Url.URLSearchParams.get("nextToken", params)}
      />
    </Providers.Apollo>;
  | _ => ReasonReact.string("Nothing here!")
  };
};
