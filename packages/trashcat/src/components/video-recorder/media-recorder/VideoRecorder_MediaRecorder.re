open Externals;

type error = [ | `InvalidPhaseTransition | `MediaRecorderError];

/** FIXME: graphql_ppx breaks on inline record definitions, ideally these are in `t` directly. **/
type recording = {stream: MediaStream.t};
type geolocation = {
  data: File.t,
  objectUrl: string,
};
type review = {
  position: Geolocation.position,
  data: File.t,
  objectUrl: string,
};
type complete = {
  data: File.t,
  position: Geolocation.position,
};

[@bs.deriving accessors]
type t =
  | PhaseGetUserMedia
  | PhaseRecording(recording)
  | PhaseGetGeolocation(geolocation)
  | PhaseReview(review)
  | PhaseError(error);

[@bs.deriving accessors]
type action =
  | SetPhase(t)
  | SetPhaseGetUserMedia
  | SetPhaseError(error)
  | SetPhaseRecording(MediaStream.t)
  | SetPhaseReview(Geolocation.position)
  | SetPhaseGetGeolocation(File.t);

[@react.component]
let make = (~mimeType, ~onFile, ~isActive) => {
  let (geolocationPermission, onGeolocationPrompt, _) =
    Service.Permission.Geolocation.use();

  let ((phaseState, prevPhaseState), dispatchPhaseAction) =
    React.useReducer(
      ((state, _prevState), action) => {
        let nextState =
          switch (action) {
          | SetPhase(phase) => phase
          | SetPhaseGetUserMedia => PhaseGetUserMedia
          | SetPhaseError(error) => PhaseError(error)
          | SetPhaseRecording(stream) => PhaseRecording({stream: stream})
          | SetPhaseReview(position) =>
            switch (state) {
            | PhaseGetGeolocation({data, objectUrl}) =>
              PhaseReview({position, data, objectUrl})
            | _ => PhaseError(`InvalidPhaseTransition)
            }
          | SetPhaseGetGeolocation(data) =>
            switch (state, geolocationPermission) {
            | (PhaseRecording(_), PermissionGranted(Some(position))) =>
              PhaseReview({
                position,
                data,
                objectUrl: Webapi.Url.createObjectURL(data),
              })
            | (PhaseRecording(_), _) =>
              PhaseGetGeolocation({
                data,
                objectUrl: Webapi.Url.createObjectURL(data),
              })
            | _ => PhaseError(`InvalidPhaseTransition)
            }
          };
        (nextState, state);
      },
      (PhaseGetUserMedia, PhaseGetUserMedia),
    );

  /** As we transition through phases, execute side effects **/
  let _ =
    React.useEffect2(
      () => {
        let handlePhaseTransition = (prevPhaseState, phaseState) =>
          switch (prevPhaseState, phaseState) {
          | (PhaseGetUserMedia, _) => ()
          | (PhaseError(_), _) => ()
          | (PhaseRecording({stream}), _) =>
            let _ =
              stream
              ->MediaStream.getTracks
              ->Belt.Array.forEach(MediaStream.Track.stop);
            ();
          | (PhaseGetGeolocation({objectUrl}), None) =>
            let _ = Webapi.Url.revokeObjectURL(objectUrl);
            ();
          | (PhaseGetGeolocation(_), Some(_)) =>
            let _ = onGeolocationPrompt();
            ();
          | (PhaseReview({objectUrl}), _) =>
            let _ = Webapi.Url.revokeObjectURL(objectUrl);
            ();
          };
        if (prevPhaseState !== phaseState) {
          let _ = handlePhaseTransition(prevPhaseState, Some(phaseState));
          ();
        };
        Some(() => handlePhaseTransition(phaseState, None));
      },
      (phaseState, prevPhaseState),
    );

  /**
    * Geolocation may update after initital prompt due to refresh,
    * propagate into state.
    */
  let _ =
    React.useEffect1(
      () => {
        let _ =
          switch (phaseState, geolocationPermission) {
          | (
              PhaseReview({position} as phase),
              PermissionGranted(Some(newPosition)),
            )
              when
                Geolocation.(
                  timestampGet(newPosition) > timestampGet(position)
                ) =>
            let _ =
              {...phase, position: newPosition}
              ->phaseReview
              ->setPhase
              ->dispatchPhaseAction;
            ();
          | _ => ()
          };
        None;
      },
      [|geolocationPermission|],
    );

  let handleReviewApprove = () =>
    switch (phaseState) {
    | PhaseReview(review) =>
      let _ = setPhaseGetUserMedia->dispatchPhaseAction;
      let _ =
        onFile(
          ~file=review.data,
          ~location=review.position->Geolocation.coordsGet,
        );
      ();
    | _ =>
      let _ = `InvalidPhaseTransition->setPhaseError->dispatchPhaseAction;
      ();
    };

  let handleReviewReject = () =>
    switch (phaseState) {
    | PhaseReview(_review) =>
      let _ = setPhaseGetUserMedia->dispatchPhaseAction;
      ();
    | _ =>
      let _ = `InvalidPhaseTransition->setPhaseError->dispatchPhaseAction;
      ();
    };

  let handleGrantedUserMedia = stream =>
    stream->setPhaseRecording->dispatchPhaseAction;

  let handleGrantedGeolocation = position =>
    position->setPhaseReview->dispatchPhaseAction;

  let handleRecordingComplete = (~blob) =>
    blob->setPhaseGetGeolocation->dispatchPhaseAction;

  let handleRecordingError = _error =>
    `MediaRecorderError->setPhaseError->dispatchPhaseAction;

  switch (phaseState) {
  | PhaseGetUserMedia =>
    <MediaRecorder_PhaseGetUserMedia onGranted=handleGrantedUserMedia />
  | PhaseGetGeolocation(_) =>
    <MediaRecorder_PhaseGetGeolocation
      onGranted=handleGrantedGeolocation
      onPrompt=onGeolocationPrompt
      permission=geolocationPermission
    />
  | PhaseRecording({stream}) =>
    <MediaRecorder_PhaseRecording
      stream
      onComplete=handleRecordingComplete
      onError=handleRecordingError
      mimeType
    />
  | PhaseReview({objectUrl}) =>
    let src =
      [|(objectUrl, mimeType)|]->VideoSurface.srcElement;
    <MediaRecorder_PhaseReview
      onApprove=handleReviewApprove
      onReject=handleReviewReject
      src
    />;
  | PhaseError(_) => <Error />
  };
};
