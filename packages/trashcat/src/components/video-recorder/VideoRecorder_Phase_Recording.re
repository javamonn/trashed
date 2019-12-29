open Externals;
open Lib.Styles;
open Lib.Utils;

module State = {
  type error = [ | `InvalidState | `MediaRecorderError];

  type t =
    | NotStarted
    | InProgress(array(File.t))
    | Complete(File.t)
    | Error(error);

  let toString =
    fun
    | NotStarted => "not started"
    | InProgress(_) => "in progress"
    | Complete(_) => "complete"
    | Error(_) => "error";

  type action =
    | SetState(t)
    | RecorderEvent((MediaRecorder.Event.t, Dom.event));
};

module TimerProgress = {
  let targetMs = 5 * 1000;

  let isComplete = time => time === targetMs;
  let percent = time => float_of_int(time) /. float_of_int(targetMs);
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

  let (time, startTimer, stopTimer, resetTimer) =
    Hook.Timer.use(~interval=500);

  let (state, dispatch) =
    React.useReducer(
      (state, action) =>
        switch (action, state) {
        | (State.SetState(newState), _) => newState
        | (RecorderEvent((MediaRecorder.Event.Start, ev)), State.NotStarted) =>
          State.InProgress([||])
        | (RecorderEvent((MediaRecorder.Event.Start, ev)), _) =>
          State.Error(`InvalidState)
        | (
            RecorderEvent((MediaRecorder.Event.DataAvailable, ev)),
            State.InProgress(data),
          ) =>
          let eventData =
            ev
            ->MediaRecorder.Event.toDataAvailable
            ->MediaRecorder.Event.dataGet;
          let _ = Js.Array.push(eventData, data);
          state;
        | (
            RecorderEvent((MediaRecorder.Event.Stop, ev)),
            State.InProgress(data),
          ) =>
          if (time->TimerProgress.isComplete) {
            let blob =
              File.makeBlob(
                data,
                File.blobOptions(~type_=mimeType->VideoSurface.mimeTypeToJs),
              );
            Complete(blob);
          } else {
            NotStarted;
          }
        | (RecorderEvent((MediaRecorder.Event.Error, _)), ev) =>
          Error(`MediaRecorderError)
        | _ => Error(`InvalidState)
        },
      NotStarted,
    );
  let _ =
    React.useEffect1(
      () => {
        let _ =
          switch (state, TimerProgress.isComplete(time)) {
          | (InProgress(_), true) =>
            let _ = recorder->MediaRecorder.stop;
            let _ = stopTimer();
            ();
          | (_, true) =>
            let _ = stopTimer();
            ();
          | _ => ()
          };
        None;
      },
      [|time|],
    );

  let _ =
    React.useEffect1(
      () => {
        let _ =
          switch (state) {
          | Complete(blob) => onComplete(~blob)
          | Error(error) => onError(error)
          | NotStarted => resetTimer()
          | _ => ()
          };
        None;
      },
      [|state|],
    );

  let handleTouchStart = ev => {
    let _ = ev->ReactEvent.Synthetic.preventDefault;
    let browser = Lib.Constants.browser->Bowser.getBrowserName;
    switch (state, browser) {
    | (NotStarted, Some(`Safari)) =>
      let _ = recorder->MediaRecorder.start;
      let _ = startTimer();
      let _ =
        MediaRecorder.dispatchEvent(
          Webapi.Dom.Event.make("start"),
          recorder,
        );
      ();
    | (NotStarted, _) =>
      let _ = recorder->MediaRecorder.start;
      let _ = startTimer();
      ();
    | _ => ()
    };
  };

  let handleTouchEnd = ev => {
    switch (state) {
    | InProgress(_) =>
      let _ = recorder->MediaRecorder.stop;
      let _ = stopTimer();
      ();
    };
    ();
  };

  let _ =
    React.useEffect0(() => {
      let handleRecorderEvent = (type_, ev) => {
        let _ = dispatch(State.RecorderEvent((type_, ev)));
        ();
      };
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
          | InProgress(_) => recorder->MediaRecorder.stop
          | _ => ()
          };
        ();
      };
      Some(onUnmount);
    });

  <div
    className={cn(["w-full", "h-full", "relative", "overflow-hidden"])}
    onTouchStart=handleTouchStart
    onTouchEnd=handleTouchEnd>
    <ScreenBottomEdgeAligned className={cn(["h-5", "m-8"])}>
      <Progress
        className={cn(["w-full", "h-full"])}
        value={time->TimerProgress.percent->Js.Float.toString}
      />
    </ScreenBottomEdgeAligned>
    <VideoSurface src={stream->VideoSurface.srcObject} autoPlay=true />
  </div>;
};
