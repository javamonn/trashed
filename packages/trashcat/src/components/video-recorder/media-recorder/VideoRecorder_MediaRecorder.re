open Externals;
open Lib.Styles;
open Lib.Utils;

module PhaseState = {
  module Recording = {
    [@bs.deriving abstract]
    type t = {stream: MediaStream.t};
    let make = t;
  };

  module GetGeolocation = {
    [@bs.deriving abstract]
    type t = {
      data: File.t,
      objectUrl: string,
    };
    let make = t;
  };

  module Review = {
    [@bs.deriving abstract]
    type t = {
      coordinates: Geolocation.coordinates,
      data: File.t,
      objectUrl: string,
    };
    let make = t;
  };

  module Complete = {
    [@bs.deriving abstract]
    type t = {
      data: File.t,
      coordinates: Geolocation.coordinates,
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
    | PhaseGetUserMedia
    | PhaseRecording(Recording.t)
    | PhaseGetGeolocation(GetGeolocation.t)
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

  PhaseState.(
    switch (p) {
    | PhaseGetUserMedia => ()
    | PhaseError(_) => ()
    | PhaseComplete(_) => ()
    | PhaseRecording(recording) =>
      let _ = recording->Recording.streamGet->cleanupMediaStream;
      ();
    | PhaseGetGeolocation(getGeolocation) when full =>
      let _ =
        getGeolocation
        ->GetGeolocation.objectUrlGet
        ->Webapi.Url.revokeObjectURL;
      ();
    | PhaseGetGeolocation(_) => ()
    | PhaseReview(review) =>
      let _ = review->Review.objectUrlGet->Webapi.Url.revokeObjectURL;
      ();
    }
  );
};

[@react.component]
let make = (~mimeType, ~onFile, ~isActive) => {
  let ((phaseState, prevPhaseState), dispatchPhaseAction) =
    React.useReducer(
      ((state, _prevState), action) =>
        PhaseState.(
          switch (action) {
          | SetPhase(p) => (p, state)
          }
        ),
      (PhaseGetUserMedia, PhaseGetUserMedia),
    );

  let _ =
    React.useEffect0(() => Some(() => cleanupPhase(~full=true, phaseState)));

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

  let handleReviewApprove = () =>
    switch (phaseState) {
    | PhaseReview(review) =>
      let _ = PhaseState.(phaseGetUserMedia->setPhase->dispatchPhaseAction);

      let _ =
        PhaseState.(
          onFile(
            ~file=review->Review.dataGet,
            ~location=review->Review.coordinatesGet,
          )
        );
      ();
    | _ => ()
    };

  let handleReviewReject = () =>
    switch (phaseState) {
    | PhaseReview(_review) =>
      let _ = PhaseState.(phaseGetUserMedia->setPhase->dispatchPhaseAction);
      ();
    | _ => ()
    };

  let handleGetUserMedia = () => {
    MediaDevices.MediaConstraints.(
      make(~audio=true, ~video=makeVideo(~facingMode="environment"), ())
    )
    |> MediaDevices.getUserMedia
    |> Js.Promise.then_(stream => {
         let _ =
           PhaseState.Recording.make(~stream)
           ->PhaseState.phaseRecording
           ->PhaseState.setPhase
           ->dispatchPhaseAction;

         Permissions.Status.make(~state=`Granted)
         ->Js.Option.some
         ->Js.Promise.resolve;
       })
    |> Js.Promise.catch(_ex =>
         /** FIXME: may have errored for other reasons */
         (
           Permissions.Status.make(~state=`Denied)
           ->Js.Option.some
           ->Js.Promise.resolve
         )
       );
  };

  let handleGetUserMediaGranted = () => {
    switch (phaseState) {
    | PhaseGetUserMedia =>
      let _ = handleGetUserMedia();
      ();
    | _ => ()
    };
  };

  let handleGetGeolocation = () =>
    Js.Promise.make((~resolve, ~reject) =>
      Geolocation.getCurrentPosition(
        position => {
          let _ =
            PhaseState.(
              switch (phaseState) {
              | PhaseGetGeolocation(getGeolocation) =>
                Review.make(
                  ~coordinates=position->Geolocation.coordsGet,
                  ~data=getGeolocation->GetGeolocation.dataGet,
                  ~objectUrl=getGeolocation->GetGeolocation.objectUrlGet,
                )
                ->phaseReview
                ->setPhase
                ->dispatchPhaseAction
              | _ => ()
              }
            );
          let _ =
            Permissions.Status.make(~state=`Granted)
            ->Js.Option.some
            ->wrapBs(resolve);
          ();
        },
        _positionError => {
          /** FIXME: may have errored for other reasons */
          let _ =
            Permissions.Status.make(~state=`Denied)
            ->Js.Option.some
            ->wrapBs(resolve);
          ();
        },
        Geolocation.currentPositionOptions(~enableHighAccuracy=true, ()),
      )
    );

  let handleGetGeolocationGranted = () =>
    switch (phaseState) {
    | PhaseGetGeolocation(_) =>
      let _ = handleGetGeolocation();
      ();
    | _ => ()
    };

  let handleRecordingComplete = (~blob) =>
    switch (phaseState) {
    | PhaseRecording(_) =>
      let objectUrl = Webapi.Url.createObjectURL(blob);
      let _ =
        PhaseState.(
          GetGeolocation.make(~data=blob, ~objectUrl)
          ->phaseGetGeolocation
          ->setPhase
          ->dispatchPhaseAction
        );
      ();
    | _ => ()
    };

  let handleRecordingError = _error =>
    switch (phaseState) {
    | PhaseRecording(_) =>
      let _ =
        PhaseState.(
          `MediaRecorderError->phaseError->setPhase->dispatchPhaseAction
        );
      ();
    | _ => ()
    };

  PhaseState.(
    switch (phaseState) {
    | PhaseGetUserMedia =>
      <MediaRecorder_PhaseGetUserMedia
        onGetUserMedia=handleGetUserMedia
        onGetUserMediaGranted=handleGetUserMediaGranted
      />
    | PhaseGetGeolocation(_) =>
      <MediaRecorder_PhaseGetGeolocation
        onGetGeolocation=handleGetGeolocation
        onGetGeolocationGranted=handleGetGeolocationGranted
      />
    | PhaseRecording(state) =>
      <MediaRecorder_PhaseRecording
        stream={state->PhaseState.Recording.streamGet}
        onComplete=handleRecordingComplete
        onError=handleRecordingError
        mimeType
      />
    | PhaseReview(state) =>
      let url = state->Review.objectUrlGet;
      let src = [|(url, mimeType)|]->VideoSurface.srcElement->Js.Option.some;
      <MediaRecorder_PhaseReview
        onApprove=handleReviewApprove
        onReject=handleReviewReject
        src
      />;
    | PhaseComplete(_) => React.null
    }
  );
};
