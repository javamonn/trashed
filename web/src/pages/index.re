open Styles;

let component = ReasonReact.statelessComponent("index");

let make = (_children) => {
  ...component,
  render: (_self) =>
    <Layout>
      <div className={cn([
        "flex-auto",
        "flex",
        "justify-center",
        "items-center",
        "bg-black"
      ])}>
        <div className={cn(["white", "f1", "ff-lobster"])}>
          { ReasonReact.string("trashed.today") }
        </div>
      </div>
    </Layout>
};

let default = ReasonReact.wrapReasonForJs(
  ~component, 
  jsProps => make([||])
)
