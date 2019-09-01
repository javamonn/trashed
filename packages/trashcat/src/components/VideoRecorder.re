open Externals;
open Lib.Styles;
open Lib.Utils;

module PhaseState = {
  module Initialized = {
    [@bs.deriving abstract]
    type t = {
      stream: MediaStream.t,
      recorder: MediaRecorder.t,
    };
    let make = t;
  };

  module Recording = {
    [@bs.deriving abstract]
    type t = {
      stream: MediaStream.t,
      recorder: MediaRecorder.t,
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
    | PhaseError(error);

  [@bs.deriving accessors]
  type action =
    | SetPhase(t)
    | HandleRecorderEvent((MediaRecorder.Event.t, Dom.event));
};

[@react.component]
let make = () => {
  let (phaseState, dispatchPhaseAction) =
    React.useReducer(
      (state, action) =>
        PhaseState.(
          switch (action) {
          | SetPhase(p) => p
          | HandleRecorderEvent((MediaRecorder.Event.Start, _ev)) =>
            switch (state) {
            | PhaseInitialized(initialized) =>
              Recording.make(
                ~stream=initialized->Initialized.streamGet,
                ~recorder=initialized->Initialized.recorderGet,
                ~data=[||],
              )
              ->phaseRecording
            | _ => PhaseError(`InvalidPhaseTransitionAttempted_Start)
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
              state;
            | _ => PhaseError(`InvalidPhaseState)
            }
          | HandleRecorderEvent((MediaRecorder.Event.Stop, _ev)) =>
            switch (state) {
            | PhaseRecording(recording) =>
              let blob =
                File.makeBlob(
                  recording->Recording.dataGet,
                  File.blobOptions(~type_="video/webm"),
                );
              let objectUrl = Webapi.Url.createObjectURL(blob);
              Review.make(~data=blob, ~objectUrl)->phaseReview;
            | _ => PhaseError(`InvalidPhaseTransitionAttempted_Stop)
            }
          | HandleRecorderEvent((MediaRecorder.Event.Error, _ev)) =>
            PhaseError(`MediaRecorderError)
          }
        ),
      PhaseState.PhaseGettingUserMedia,
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
             let recorder =
               MediaRecorder.(
                 make(
                   s,
                   options(~mimeType="video/webm") |> Js.Nullable.return,
                 )
                 |> doto(
                      addEventListener(
                        "dataavailable",
                        onMediaRecorderEvent(Event.DataAvailable),
                      ),
                    )
                 |> doto(
                      addEventListener(
                        "start",
                        onMediaRecorderEvent(Event.Start),
                      ),
                    )
                 |> doto(
                      addEventListener(
                        "stop",
                        onMediaRecorderEvent(Event.Stop),
                      ),
                    )
                 |> doto(
                      addEventListener(
                        "error",
                        onMediaRecorderEvent(Event.Error),
                      ),
                    )
               );
             let _ =
               PhaseState.Initialized.make(~stream=s, ~recorder)
               ->PhaseState.phaseInitialized
               ->PhaseState.setPhase
               ->dispatchPhaseAction;
             Js.Promise.resolve();
           });
      None;
    });

  let src =
    PhaseState.(
      switch (phaseState) {
      | PhaseGettingUserMedia => None
      | PhaseInitialized(initialized) =>
        initialized
        ->Initialized.streamGet
        ->VideoSurface.srcObject
        ->Js.Option.some
      | PhaseRecording(recording) =>
        recording->Recording.streamGet->VideoSurface.srcObject->Js.Option.some
      | PhaseReview(review) =>
        review->Review.objectUrlGet->VideoSurface.srcUrl->Js.Option.some
      | PhaseError(_) => None
      }
    );

  let handleTouchEnd = _ev =>
    PhaseState.(
      switch (phaseState) {
      | PhaseInitialized(initialized) =>
        let _ = initialized->Initialized.recorderGet->MediaRecorder.start;
        ();
      | PhaseRecording(recording) =>
        let _ = recording->Recording.recorderGet->MediaRecorder.stop;
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
