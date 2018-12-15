open Styles;

let component = ReasonReact.statelessComponent("index");

let make = _children => {
  ...component,
  render: _self =>
    <Layout>
      <div
        className={cn([
          "flex-auto",
          "flex",
          "flex-column",
          "justify-center",
          "items-center",
          "bg-black",
        ])}>
        <div className={cn(["white", "f1", "ff-lobster", "mb3"])}>
          {ReasonReact.string("trashed.today")}
        </div>
        <div className={cn(["white", "f4", "ff-lobster"])}>
          {ReasonReact.string("dumpster diving for the digital age")}
        </div>
      </div>
    </Layout>,
};

let default = ReasonReact.wrapReasonForJs(~component, jsProps => make([||]));
