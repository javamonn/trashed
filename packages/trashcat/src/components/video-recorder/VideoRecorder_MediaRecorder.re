open Externals;
open Lib.Styles;
open Lib.Utils;

module PhaseState = {
  module Initialized = {
    [@bs.deriving abstract]
    type t = {
      stream: MediaStream.t,
      recorder: MediaRecorder.t,
      eventListeners: array((string, Dom.event => unit)),
    };
    let make = t;
  };

  module Recording = {
    [@bs.deriving abstract]
    type t = {
      stream: MediaStream.t,
      recorder: MediaRecorder.t,
      eventListeners: array((string, Dom.event => unit)),
      data: array(File.t),
    };
    let make = t;
  };

  module Review = {
    [@bs.deriving abstract]
    type t = {
      data: File.t,
      objectUrl: string,
    };
    let make = t;
  };

  module Complete = {
    [@bs.deriving abstract]
    type t = {data: File.t};
    let make = t;
  };

  type error = [
    | `InvalidPhaseTransitionAttempted_Start
    | `InvalidPhaseTransitionAttempted_Stop
    | `InvalidPhaseState
    | `MediaRecorderError
  ];

  [@bs.deriving accessors]
  type t =
    | PhaseGettingUserMedia
    | PhaseInitialized(Initialized.t)
    | PhaseRecording(Recording.t)
    | PhaseReview(Review.t)
    | PhaseComplete(Complete.t)
    | PhaseError(error);

  [@bs.deriving accessors]
  type action =
    | SetPhase(t)
    | HandleRecorderEvent((MediaRecorder.Event.t, Dom.event));
};

let cleanupPhase = (~full=false, p) => {
  let cleanupMediaStream = stream => {
    let _ =
      stream
      ->MediaStream.getTracks
      ->Belt.Array.forEach(MediaStream.Track.stop);
    ();
  };

  let cleanupRecorder = (~recorder, ~eventListeners) => {
    let _ =
      eventListeners->Belt.Array.forEach(((t, l)) =>
        MediaRecorder.removeEventListener(t, l, recorder)
      );
    let _ =
      switch (recorder->MediaRecorder.state) {
      | Some(`Recording)
      | Some(`Paused) => recorder->MediaRecorder.stop
      | _ => ()
      };
    ();
  };

  PhaseState.(
    switch (p) {
    | PhaseGettingUserMedia =>
      /**
       * FIXME: hold the promise and immediately end the stream
       * once resolved
       */
      ()
    | PhaseError(_) => ()
    | PhaseComplete(_) => ()
    | PhaseInitialized(_) when !full => ()
    | PhaseInitialized(initialized) when full =>
      let _ = initialized->Initialized.streamGet->cleanupMediaStream;
      let _ =
        cleanupRecorder(
          ~recorder=initialized->Initialized.recorderGet,
          ~eventListeners=initialized->Initialized.eventListenersGet,
        );
      ();
    | PhaseRecording(recording) =>
      let _ = recording->Recording.streamGet->cleanupMediaStream;
      let _ =
        cleanupRecorder(
          ~recorder=recording->Recording.recorderGet,
          ~eventListeners=recording->Recording.eventListenersGet,
        );
      ();
    | PhaseReview(review) =>
      let _ = review->Review.objectUrlGet->Webapi.Url.revokeObjectURL;
      ();
    }
  );
};

[@react.component]
let make = (~mimeType, ~onFile) => {
  let ((phaseState, prevPhaseState), dispatchPhaseAction) =
    React.useReducer(
      ((state, prevState), action) =>
        PhaseState.(
          switch (action) {
          | SetPhase(p) => (p, state)
          | HandleRecorderEvent((MediaRecorder.Event.Start, _ev)) =>
            switch (state) {
            | PhaseInitialized(initialized) =>
              let nextPhaseState =
                Recording.make(
                  ~stream=initialized->Initialized.streamGet,
                  ~recorder=initialized->Initialized.recorderGet,
                  ~eventListeners=initialized->Initialized.eventListenersGet,
                  ~data=[||],
                )
                ->phaseRecording;
              (nextPhaseState, state);
            | _ => (
                PhaseError(`InvalidPhaseTransitionAttempted_Start),
                state,
              )
            }
          | HandleRecorderEvent((MediaRecorder.Event.DataAvailable, ev)) =>
            switch (state) {
            | PhaseRecording(recording) =>
              let eventData =
                ev
                ->MediaRecorder.Event.toDataAvailable
                ->MediaRecorder.Event.dataGet;
              let phaseData = recording->Recording.dataGet;
              let _ = Js.Array.push(eventData, phaseData);
              (state, state);
            | _ => (PhaseError(`InvalidPhaseState), state)
            }
          | HandleRecorderEvent((MediaRecorder.Event.Stop, _ev)) =>
            switch (state) {
            | PhaseRecording(recording) =>
              let blob =
                File.makeBlob(
                  recording->Recording.dataGet,
                  File.blobOptions(~type_=mimeType),
                );
              let objectUrl = Webapi.Url.createObjectURL(blob);
              let nextPhaseState =
                Review.make(~data=blob, ~objectUrl)->phaseReview;
              (nextPhaseState, state);
            | _ => (PhaseError(`InvalidPhaseTransitionAttempted_Stop), state)
            }
          | HandleRecorderEvent((MediaRecorder.Event.Error, _ev)) => (
              PhaseError(`MediaRecorderError),
              state,
            )
          }
        ),
      (PhaseState.PhaseGettingUserMedia, PhaseState.PhaseGettingUserMedia),
    );

  let _ =
    React.useEffect0(() => {
      let _ =
        MediaDevices.MediaConstraints.(
          make(~audio=true, ~video=makeVideo(~facingMode="environment"), ())
        )
        |> MediaDevices.getUserMedia
        |> Js.Promise.then_(s => {
             let onMediaRecorderEvent = (eventType, event) => {
               let _ =
                 PhaseState.handleRecorderEvent((eventType, event))
                 ->dispatchPhaseAction;
               ();
             };
             let eventListeners = [|
               (
                 "dataavailable",
                 onMediaRecorderEvent(MediaRecorder.Event.DataAvailable),
               ),
               ("start", onMediaRecorderEvent(MediaRecorder.Event.Start)),
               ("stop", onMediaRecorderEvent(MediaRecorder.Event.Stop)),
               ("error", onMediaRecorderEvent(MediaRecorder.Event.Error)),
             |];
             let recorder =
               MediaRecorder.make(
                 s,
                 MediaRecorder.options(~mimeType) |> Js.Nullable.return,
               );
             let _ =
               eventListeners->Belt.Array.reduce(
                 recorder, (recorder, (ev, l)) =>
                 doto(MediaRecorder.addEventListener(ev, l), recorder)
               );
             let _ =
               PhaseState.Initialized.make(
                 ~stream=s,
                 ~recorder,
                 ~eventListeners,
               )
               ->PhaseState.phaseInitialized
               ->PhaseState.setPhase
               ->dispatchPhaseAction;
             Js.Promise.resolve();
           });

      let onCleanup = () => cleanupPhase(~full=true, phaseState);

      Some(onCleanup);
    });

  let _ =
    React.useEffect2(
      () => {
        if (prevPhaseState !== phaseState) {
          let _ = cleanupPhase(prevPhaseState);
          ();
        };
        None;
      },
      (phaseState, prevPhaseState),
    );

  let src =
    PhaseState.(
      switch (phaseState) {
      | PhaseGettingUserMedia => None
      | PhaseComplete(_) => None
      | PhaseInitialized(initialized) =>
        initialized
        ->Initialized.streamGet
        ->VideoSurface.srcObject
        ->Js.Option.some
      | PhaseRecording(recording) =>
        recording->Recording.streamGet->VideoSurface.srcObject->Js.Option.some
      | PhaseReview(review) =>
        let url = review->Review.objectUrlGet;
        [|(url, mimeType)|]->VideoSurface.srcElement->Js.Option.some;
      | PhaseError(_) => None
      }
    );

  let handleTouchEnd = _ev =>
    PhaseState.(
      switch (phaseState) {
      | PhaseInitialized(initialized) =>
        let recorder = initialized->Initialized.recorderGet;
        let _ = recorder->MediaRecorder.start;
        /**
         * Safari does not fire a `start` event, so we manually dispatch one.
         */
        let _ =
          switch (Lib.Constants.browser->Bowser.getBrowserName) {
          | Some(`Safari) =>
            let _ =
              MediaRecorder.dispatchEvent(
                Webapi.Dom.Event.make("start"),
                recorder,
              );
            ();
          | _ => ()
          };
        ();
      | PhaseRecording(recording) =>
        let _ = recording->Recording.recorderGet->MediaRecorder.stop;
        ();
      | PhaseReview(review) =>
        let _ =
          Complete.make(~data=review->Review.dataGet)
          ->phaseComplete
          ->PhaseState.setPhase
          ->dispatchPhaseAction;
        ();
      | _ => ()
      }
    );

  let controls =
    PhaseState.(
      switch (phaseState) {
      | PhaseReview(_) => true
      | _ => false
      }
    );

  <div
    className={cn(["w-screen", "h-screen", "relative"])}
    onTouchEnd=handleTouchEnd>
    <VideoSurface ?src autoPlay=true controls />
  </div>;
};
