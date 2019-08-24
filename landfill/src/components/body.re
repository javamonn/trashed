open Styles;

let component = ReasonReact.statelessComponent("body");

let make = (~text, ~className=?, _children) => {
  ...component,
  render: _self =>
    <span className={cn(["ff-poppins", "f13", "lh22", Cn.unpack(className)])}>
      {ReasonReact.string(text)}
    </span>
}
