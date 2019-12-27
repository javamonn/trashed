open Externals;
open VideoRecorder_Phase;

type error = [ | `InvalidPhaseTransition | `MediaRecorderError];

/** FIXME: graphql_ppx breaks on inline record definitions, ideally these are in `t` directly. **/
type recording = {stream: MediaStream.t};
type geolocation = {
  data: File.t,
  objectUrl: string,
  stream: MediaStream.t,
};
type review = {
  position: Geolocation.position,
  data: File.t,
  objectUrl: string,
  stream: MediaStream.t,
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
            | PhaseGetGeolocation({data, objectUrl, stream}) =>
              PhaseReview({position, data, objectUrl, stream})
            | _ => PhaseError(`InvalidPhaseTransition)
            }
          | SetPhaseGetGeolocation(data) =>
            switch (state, geolocationPermission) {
            | (PhaseRecording({stream}), PermissionGranted(Some(position))) =>
              PhaseReview({
                stream,
                position,
                data,
                objectUrl: Webapi.Url.createObjectURL(data),
              })
            | (PhaseRecording({stream}), _) =>
              PhaseGetGeolocation({
                data,
                stream,
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
          | (PhaseRecording(_), _) => ()
          | (PhaseGetGeolocation(_), _) =>
            let _ = onGeolocationPrompt();
            ();
          | (PhaseReview({objectUrl}), _) =>
            let _ = Webapi.Url.revokeObjectURL(objectUrl);
            ();
          };
        if (prevPhaseState !== phaseState) {
          let _ = handlePhaseTransition(prevPhaseState, phaseState);
          ();
        };
        None;
      },
      (phaseState, prevPhaseState),
    );
  let _ =
    React.useEffect0(() => {
      let stopMediaStream = stream =>
        stream
        ->MediaStream.getTracks
        ->Belt.Array.forEach(MediaStream.Track.stop);

      Some(
        () => {
          switch (phaseState) {
          | PhaseRecording({stream}) => stopMediaStream(stream)
          | PhaseGetGeolocation({stream}) => stopMediaStream(stream)
          | PhaseReview({objectUrl, stream}) =>
            let _ = stopMediaStream(stream);
            let _ = Webapi.Url.revokeObjectURL(objectUrl);
            ();
          | _ => ()
          }
        },
      );
    });

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
    | PhaseReview({stream, data, position}) =>
      let _ = stream->setPhaseRecording->dispatchPhaseAction;
      let _ = onFile(~file=data, ~location=position->Geolocation.coordsGet);
      ();
    | _ =>
      let _ = `InvalidPhaseTransition->setPhaseError->dispatchPhaseAction;
      ();
    };

  let handleReviewReject = () =>
    switch (phaseState) {
    | PhaseReview({stream}) =>
      let _ = stream->setPhaseRecording->dispatchPhaseAction;
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
    <PermissionPromptCamera.MediaRecorder onGranted=handleGrantedUserMedia />
  | PhaseGetGeolocation(_) =>
    <PermissionPromptGeolocation
      onGranted=handleGrantedGeolocation
      onPrompt=onGeolocationPrompt
      permission=geolocationPermission
    />
  | PhaseRecording({stream}) =>
    <Recording
      stream
      onComplete=handleRecordingComplete
      onError=handleRecordingError
      mimeType
    />
  | PhaseReview({objectUrl}) =>
    let src = [|(objectUrl, mimeType)|]->VideoSurface.srcElement;
    <Review onApprove=handleReviewApprove onReject=handleReviewReject src />;
  | PhaseError(_) => <Error />
  };
};
