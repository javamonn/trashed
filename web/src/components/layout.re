open Styles;

[@bs.val] external require : string => unit = "";

require("tachyons/css/tachyons.min.css");
require("./layout.css");
require("typeface-lobster");


let component = ReasonReact.statelessComponent("Layout")

let make = (children) => {
  ...component,
  render: (_self) =>
    <div className={cn(["absolute", "absolute--fill", "flex"])}> 
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
