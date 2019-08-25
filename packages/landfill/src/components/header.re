open Styles;

let component = ReasonReact.statelessComponent("header");

let make = (~className=?, ~text, _children) => {
  ...component,
  render: _self =>
    <span className={cn(["ff-abril-fatface", "b", "f18", Cn.unpack(className)])}>
      {ReasonReact.string(text)}
    </span>
}
