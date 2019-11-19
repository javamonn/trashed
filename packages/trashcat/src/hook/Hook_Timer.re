let use = (~interval) => {
  let timer = React.useRef(None);
  let (time, setTime) = React.useState(() => 0);

  let start = () =>
    switch (timer->React.Ref.current) {
    | Some(timer) => ()
    | None =>
      setTime(_time => 0);
      timer->React.Ref.setCurrent(
        Some(
          Js.Global.setInterval(
            () => setTime(time => time + interval),
            interval,
          ),
        ),
      );
    };

  let stop = () =>
    switch (timer->React.Ref.current) {
    | Some(intervalId) =>
      let _ = intervalId->Js.Global.clearInterval;
      let _ = timer->React.Ref.setCurrent(None);
      ();
    | None => ()
    };

  let _ = React.useEffect0(() => Some(() => stop()));

  (time, start, stop);
};
