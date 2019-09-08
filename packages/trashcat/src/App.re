open Externals;
open Lib_Utils;

%raw
"import './app.pcss'";

let _ = Amplify.(inst->configure(config));

[@react.component]
let make = () => {
  let url = ReasonReactRouter.useUrl();

  switch (url.path) {
  | ["record"] =>
    <Providers.Apollo> <VideoRecorderScreen /> </Providers.Apollo>
  | _ => ReasonReact.string("Nothing here!")
  };
};
