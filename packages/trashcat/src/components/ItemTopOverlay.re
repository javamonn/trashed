open Lib.Styles;

module GeolocationRequired = {
  [@react.component]
  let make = (~permission, ~onClick, ~style=?) =>
    <div
      ?style
      className={cn([
        "absolute",
        "inset-x-0",
        "top-0",
        "h-24",
        "my-4",
        "mx-6",
        "z-10",
        "flex",
        "flex-row",
      ])}>
      <Button
        className={cn([
          "flex-auto",
          "flex",
          "items-center",
          "justify-between",
        ])}
        onClick>
        <div
          className={cn([
            "h-24",
            "w-24",
            "flex-shrink-0",
            "flex",
            "flex-auto",
            "items-center",
            "justify-center",
            "p-6",
          ])}>
          {switch (permission) {
           | Service.Permission.TransitionInProgress(_) =>
             <Progress className={cn(["w-full"])} />
           | _ =>
             <SVG className={cn(["h-full", "w-full"])} icon=SVG.geolocation />
           }}
        </div>
        <div
          className={cn([
            "text-black",
            "font-bold",
            "italic",
            "leading-none",
            "text-xl",
            "mr-6",
          ])}>
          {React.string(
             "trashed needs your location to show you nearby things.",
           )}
        </div>
      </Button>
    </div>;
};

[@react.component]
let make = (~onPromptGeolocation, ~geolocationPermission) => {
  let (_in, setIn) = React.useState(() => false);
  let timeoutId = React.useRef(None);

  let clearTimeout = () =>
    switch (timeoutId->React.Ref.current) {
    | Some(id) =>
      let _ = id->Js.Global.clearTimeout;
      let _ = timeoutId->React.Ref.setCurrent(None);
      ();
    | None => ()
    };

  let deriveIn = () =>
    switch (geolocationPermission) {
    | Service.Permission.Unknown
    | Service.Permission.PermissionGranted(Some(_)) => false
    | _ => true
    };

  let _ =
    React.useEffect0(() => {
      let newTimeoutId =
        Js.Global.setTimeout(
          () => {
            let in_ = deriveIn();
            let _ = setIn(_ => in_);
            ();
          },
          3000,
        );
      let _ = timeoutId->React.Ref.setCurrent(Some(newTimeoutId));

      Some(clearTimeout);
    });

  let _ =
    React.useEffect1(
      () => {
        let newIn = deriveIn();
        if (newIn !== _in) {
          let _ = clearTimeout();
          let _ = setIn(_ => newIn);
          ();
        };
        None;
      },
      [|geolocationPermission|],
    );

  <Grow _in appear=true>
    <GeolocationRequired
      permission=geolocationPermission
      onClick={_ev => {
        let _ = onPromptGeolocation();
        ();
      }}
    />
  </Grow>;
};
