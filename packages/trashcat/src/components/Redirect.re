[@react.component]
let make = (~to_) => {
  let _ = ReasonReactRouter.replace(to_);
  React.null;
};
