open Externals;

%raw
"import './app.pcss'";

let _ = Amplify.configure(Amplify.config);

[@react.component]
let make = () => {
  let url = ReasonReactRouter.useUrl();

  switch (url.path) {
  | ["record"] =>
    <Providers.Apollo> <VideoRecorderScreen /> </Providers.Apollo>
  | _ => ReasonReact.string("Nothing here!")
  };
};
