open Styles;

Utils.require("./button.module.css");

let component = ReasonReact.statelessComponent("Button");

let make = (~label, ~type_, _children) => {
  ...component,
  render: _self =>
    <button
      type_
      className={cn([
        "br1",
        "bg-brand",
        "pv_75",
        "ph4",
        "b",
        "f15",
        "ff-poppins"
      ])}
    >
      {label}
    </button>
}
