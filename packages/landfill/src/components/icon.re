open Styles;

module type RawReactClass = {let reactClass: ReasonReact.reactClass;};

module MakeIcon = (RawReactClass: RawReactClass) => {
  let make = (~style=?, children) =>
    ReasonReact.wrapJsForReason(
      ~reactClass=RawReactClass.reactClass,
      ~props={"style": Js.Nullable.fromOption(style)},
      children,
    );
};

module Phone =
  MakeIcon({
    [@bs.module "@material-ui/icons/Phone"]
    external reactClass: ReasonReact.reactClass = "default";
  });
