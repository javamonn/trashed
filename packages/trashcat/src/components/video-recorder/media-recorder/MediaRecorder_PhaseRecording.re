open Externals;
open Lib.Styles;
open Lib.Utils;

module State = {
  type error = [ | `InvalidState | `MediaRecorderError];

  type t =
    | NotStarted
    | InProgress(array(File.t))
    | Complete
    | Error;

  type action =
    | SetState(t);
};

module TimerProgress = {
  let targetMs = 7 * 1000;

  let isComplete = time => time === targetMs;
  let percent = time =>
    int_of_float(float_of_int(time) /. float_of_int(targetMs) *. 100.);
};

[@react.component]
let make = (~stream, ~mimeType, ~onError, ~onComplete) => {
  let (recorder, _setRecorder) =
    React.useState(() =>
      MediaRecorder.make(
        stream,
        MediaRecorder.options(~mimeType=mimeType->VideoSurface.mimeTypeToJs)
        |> Js.Nullable.return,
      )
    );

  let (state, dispatch) =
    React.useReducer(
      (state, action) =>
        switch (action) {
        | State.SetState(newState) => newState
        },
      State.NotStarted,
    );

  let (time, startTimer, stopTimer) = Hook.Timer.use(~interval=500);
  let _ =
    React.useEffect1(
      () => {
        let _ =
          switch (state, TimerProgress.isComplete(time)) {
          | (InProgress, true) =>
            let _ = recorder->MediaRecorder.stop;
            let _ = stopTimer();
            ();
          | (_, true) =>
            let _ = stopTimer();
            ();
          | _ => ()
          };
        Some(stopTimer);
      },
      [|time|],
    );

  let handleRecorderEvent = (type_, ev) =>
    switch (type_, state) {
    | (MediaRecorder.Event.Start, State.NotStarted) =>
      let _ = dispatch(State.(SetState(InProgress([||]))));
      ();
    | (MediaRecorder.Event.Start, _) =>
      let _ = onError(`InvalidState);
      let _ = dispatch(State.(SetState(Error)));
      ();
    | (MediaRecorder.Event.DataAvailable, State.InProgress(data)) =>
      let eventData =
        ev->MediaRecorder.Event.toDataAvailable->MediaRecorder.Event.dataGet;
      let _ = Js.Array.push(eventData, data);
      ();
    | (MediaRecorder.Event.Stop, State.InProgress(data)) =>
      if (inProgress->State.isRecordingComplete) {
        let blob =
          File.makeBlob(
            data,
            File.blobOptions(~type_=mimeType->VideoSurface.mimeTypeToJs),
          );
        let _ = onComplete(~blob);
        let _ = dispatch(State.(SetState(Complete)));
        ();
      } else {
        let _ = dispatch(State.(SetState(NotStarted)));
        ();
      }
    | (MediaRecorder.Event.Error, _) =>
      let _ = onError(`MediaRecorderError);
      let _ = dispatch(State.(SetState(Error)));
      ();
    | _ =>
      let _ = onError(`InvalidState);
      let _ = dispatch(State.(SetState(Error)));
      ();
    };

  let handleTouchStart = _ev => {
    let browser = Lib.Constants.browser->Bowser.getBrowserName;
    switch (state, browser) {
    | (NotStarted, Some(`Safari)) =>
      let _ = recorder->MediaRecorder.start;
      let _ =
        MediaRecorder.dispatchEvent(
          Webapi.Dom.Event.make("start"),
          recorder,
        );
      ();
    | (NotStarted, _) =>
      let _ = recorder->MediaRecorder.start;
      ();
    | _ => ()
    };
  };

  let handleTouchEnd = _ev => {
    switch (state) {
    | InProgress(_) =>
      let _ = recorder->MediaRecorder.stop;
      ();
    };
    ();
  };

  let _ =
    React.useEffect0(() => {
      let eventListeners =
        MediaRecorder.Event.(
          [|
            ("dataavailable", handleRecorderEvent(DataAvailable)),
            ("start", handleRecorderEvent(Start)),
            ("stop", handleRecorderEvent(Stop)),
            ("error", handleRecorderEvent(Error)),
          |]
        );
      let _ =
        eventListeners->Belt.Array.reduce(recorder, (recorder, (ev, l)) =>
          doto(MediaRecorder.addEventListener(ev, l), recorder)
        );
      let onUnmount = () => {
        let _ =
          eventListeners->Belt.Array.forEach(((t, l)) =>
            MediaRecorder.removeEventListener(t, l, recorder)
          );
        let _ =
          switch (state) {
          | InProgress => recorder->MediaRecorder.stop
          | _ => ()
          };
        ();
      };
      Some(onUnmount);
    });

  <div
    className={cn(["w-screen", "h-screen", "relative"])}
    onTouchStart=handleTouchStart
    onTouchEnd=handleTouchEnd>
    <div className={cn(["absolute", "inset-x-0", "top-0", "h-32", "p-8"])}>
      <Progress value={TimerProgress.percent(time)} />
    </div>
    <VideoSurface src={stream->VideoSurface.srcObject} autoPlay=true />
  </div>;
};
