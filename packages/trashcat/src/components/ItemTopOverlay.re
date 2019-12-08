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

  <Notification.GeolocationRequired
    _in
    onClick={_ => {
      let _ = onPromptGeolocation();
      ();
    }}
    permission=geolocationPermission
  />;
};
