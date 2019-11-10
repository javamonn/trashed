open Styles;

Utils.require("./button.module.css");

let component = ReasonReact.statelessComponent("Button");

let make = (~label, ~type_, ~href=?, _children) => {
  ...component,
  render: _self => {
    let comp = (
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
    );
    switch (href) {
      | Some(href) =>
        <a href>
          {comp}
        </a>
      | None => comp
    }
  }
}
