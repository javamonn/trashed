open Externals;
open Lib.Styles;

module State = {
  [@bs.deriving abstract]
  type inProgress = {
    data: array(File.t),
    progress: float,
  };

  type error = [ | `InvalidState];

  type t =
    | NotStarted
    | InProgress(inProgress)
    | Complete
    | Error;

  type action =
    | SetState(t);
};

[@react.component]
let make = (~stream, ~mimeType) => {
  let (recorder, _setRecorder) =
    React.useState(() =>
      MediaRecorder.make(
        stream,
        MediaRecorder.options(~mimeType=mimeType->VideoSurface.mimeTypeToJs)
        |> Js.Nullable.return,
      )
    );

  let (state, dispatch) =
    React.useReducer((state, action) =>
      switch (action) {
      | SetState(newState) => newState
      }
    );

  let handleRecorderEvent = (type_, ev) =>
    switch (type_, state) {
    | (MediaRecorder.Event.Start, State.NotStarted) =>
      let _ = dispatch(State.(SetState({data: [||], progress: 0.})));
      ();
    | (MediaRecorder.Event.Start, _) =>
      let _ = onError(`InvalidState_Start);
      let _ = dispatch(State.(SetState(`InvalidState_Start)));
      ();
    | (MediaRecorder.Event.DataAvailable, State.InProgress(inProgress)) => ()
    | MediaRecorder.Event.Stop => ()
    | MediaRecorder.Event.Error => ()
    };

  let onRecorderStart = () => {/** start recorder, fire event for safari **/};

  let onRecorderStop = () => {
    /**
     * stop recorder, let handleRecorderEvent figure out if reset
     * or not
     */
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
      let removeEventListeners = () =>
        eventListeners->Belt.Array.forEach(((t, l)) =>
          MediaRecorder.removeEventListener(t, l, recorder)
        );
      Some(removeEventListeners);
    });

  <div
    className={cn(["w-screen", "h-screen", "relative"])}
    onTouchEnd=handleTouchEnd>
    <VideoSurface
      src={stream->VideoSurface.srcObject->Js.Option.some}
      autoPlay=true
    />
  </div>;
};
