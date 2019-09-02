%raw
"import './app.pcss'";

[@react.component]
let make = () => {
  let url = ReasonReactRouter.useUrl();
  switch (url.path) {
  | ["record"] => <VideoRecorderScreen />
  | _ => ReasonReact.string("Nothing here!")
  };
};
