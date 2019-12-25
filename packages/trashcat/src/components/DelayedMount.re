[@react.component]
let make = (~timeout, ~paused=false, ~children) => {
  let (isMounted, setIsMounted) = React.useState(() => false);
  let timeoutId = React.useRef(None);

  let clearTimeout = () =>
    switch (timeoutId->React.Ref.current) {
    | Some(id) =>
      let _ = id->Js.Global.clearTimeout;
      let _ = timeoutId->React.Ref.setCurrent(None);
      ();
    | None => ()
    };

  let setTimeout = () => {
    let newTimeoutId =
      Js.Global.setTimeout(
        () => {
          let _ = setIsMounted(_ => true);
          ();
        },
        timeout,
      );
    let _ = timeoutId->React.Ref.setCurrent(Some(newTimeoutId));
    ();
  };

  let _ = React.useEffect0(() => {Some(clearTimeout)});

  let _ =
    React.useEffect1(
      () => {
        let _ =
          switch (timeoutId->React.Ref.current) {
          | None when !paused => setTimeout()
          | _ => ()
          };
        None;
      },
      [|paused|],
    );

  if (isMounted) {children} else {React.null};
};
