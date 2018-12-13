let component = ReasonReact.statelessComponent("index");

let make = (_children) => {
  ...component,
  render: (_self) =>
    <div>
      {ReasonReact.string("Hello, World")}
    </div>
};

let default = ReasonReact.wrapReasonForJs(
  ~component, 
  jsProps => make([||])
)
