open Lib;
open Lib.Styles;

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
    let params = Webapi.Url.URLSearchParams.make(url.search);

    let nextToken = Webapi.Url.URLSearchParams.get("nextToken", params);
    let itemId = Webapi.Url.URLSearchParams.get("itemId", params);
    let initialIdx =
      switch (rest) {
      | ["new"] => 0
      | ["search"] => 2
      | _ => 1
      };

    <Providers.Apollo>
      <ScrollSnapList.Container direction=ScrollSnapList.Horizontal initialIdx>
        <ScrollSnapList.Item direction=ScrollSnapList.Horizontal>
          <Screen.NewItem />
        </ScrollSnapList.Item>
        <ScrollSnapList.Item direction=ScrollSnapList.Horizontal>
          <Screen.ListItems ?nextToken ?itemId />
        </ScrollSnapList.Item>
        <ScrollSnapList.Item
          direction=ScrollSnapList.Horizontal
          className={cn(["bg-brandBlue"])}
        />
      </ScrollSnapList.Container>
    </Providers.Apollo>;
  | _ => ReasonReact.string("Nothing here!")
  };
};
