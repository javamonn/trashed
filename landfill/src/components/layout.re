open Styles;

Utils.require("tachyons/css/tachyons.min.css");

Utils.require("typeface-lobster");
Utils.require("typeface-poppins");
Utils.require("typeface-abril-fatface");

Utils.require("./layout.css");

let component = ReasonReact.statelessComponent("Layout")

let make = (children) => {
  ...component,
  render: (_self) =>
    <div className={cn(["absolute", "absolute--fill", "flex", "flex-column"])}> 
      ...children
    </div>
};

[@bs.deriving abstract]
type jsProps = {
  children: array(ReasonReact.reactElement)
};

let default = ReasonReact.wrapReasonForJs(
  ~component, 
  jsProps => make(jsProps->childrenGet)
);
