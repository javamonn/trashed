open Styles;

let component = ReasonReact.statelessComponent("index");

let make = _children => {
  ...component,
  render: _self =>
    <Layout>
      <div>
        <div
          className={cn([
            "flex",
            "flex-auto",
            "flex-column",
            "bg-brand",
            "ph4",
            "pv5"
          ])}>
          <div className={cn(["pd", "f36", "ff-lobster", "mb3", "mt3", "b"])}>
            {ReasonReact.string("trashed.today")}
          </div>
          <div className={cn(["pd", "f15", "ff-poppins"])}>
            {ReasonReact.string("Dumpster diving for the digital age.")}
          </div>
          <div className={cn(["mb6"])} />
          <Header className={cn(["mb3"])} text="The Manifesto" />
          <Body text="Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam vestibulum porttitor metus et convallis. Nam finibus, magna eget ultricies tristique, neque nisi dapibus urna, sit amet porta lectus velit a nunc. Fusce gravida sit amet mi laoreet vulputate. Donec laoreet sem posuere velit efficitur, ac iaculis eros venenatis. Aenean orci metus, accumsan vel finibus vel, feugiat ac quam. Etiam mattis elit fermentum dolor varius, a pharetra ex hendrerit." />
        </div>
        <div className={cn(["bg-gray-1"])}>
          <div className={cn(["ph4", "pv5", "flex", "flex-column", "bb", "b--divider-dark"])}>
            <Header className={cn(["mb3"])} text="Get Involved" />
            <Body className={cn(["mb3"])} text="Get notified when we launch. Launching in NYC soon, and other cities after. No spam, we promise." />
            <EmailForm />
          </div>
          <div className={cn(["ph4", "pv5", "flex", "flex-column"])}>
            <Header className={cn(["mb3"])} text="Follow Along" />
            <Body className={cn(["mb3"])} text="We maintain a journal documenting trash culture, our progress, and more." />
            <div>
              <Button
                label={ReasonReact.string("Read our journal")}
                type_="button"
              />
            </div>
          </div>
        </div>
      </div>
    </Layout>,
};

let default = ReasonReact.wrapReasonForJs(~component, jsProps => make([||]));
