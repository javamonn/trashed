open Externals;

type error = [ | `InvalidPhaseTransition | `MediaRecorderError];

/** FIXME: graphql_ppx breaks on inline record definitions, ideally these are in `t` directly. **/
type recording = {stream: MediaStream.t};
type geolocation = {
  data: File.t,
  objectUrl: string,
};
type review = {
  coordinates: Geolocation.coordinates,
  data: File.t,
  objectUrl: string,
};
type complete = {
  data: File.t,
  coordinates: Geolocation.coordinates,
};

[@bs.deriving accessors]
type t =
  | PhaseGetUserMedia
  | PhaseRecording(recording)
  | PhaseGetGeolocation(geolocation)
  | PhaseReview(review)
  | PhaseComplete(complete)
  | PhaseError(error);

[@bs.deriving accessors]
type action =
  | SetPhaseGetUserMedia
  | SetPhaseError(error)
  | SetPhaseRecording(MediaStream.t)
  | SetPhaseReview(Geolocation.position)
  | SetPhaseGetGeolocation(File.t);

let cleanupPhase = (~full=false, p) =>
  switch (p) {
  | PhaseGetUserMedia => ()
  | PhaseError(_) => ()
  | PhaseComplete(_) => ()
  | PhaseRecording({stream}) =>
    let _ =
      stream
      ->MediaStream.getTracks
      ->Belt.Array.forEach(MediaStream.Track.stop);
    ();
  | PhaseGetGeolocation({objectUrl}) when full =>
    let _ = Webapi.Url.revokeObjectURL(objectUrl);
    ();
  | PhaseGetGeolocation(_) => ()
  | PhaseReview({objectUrl}) =>
    let _ = Webapi.Url.revokeObjectURL(objectUrl);
    ();
  };

[@react.component]
let make = (~mimeType, ~onFile, ~isActive) => {
  let ((phaseState, prevPhaseState), dispatchPhaseAction) =
    React.useReducer(
      ((state, _prevState), action) => {
        let nextState =
          switch (action) {
          | SetPhaseGetUserMedia => PhaseGetUserMedia
          | SetPhaseError(error) => PhaseError(error)
          | SetPhaseRecording(stream) => PhaseRecording({stream: stream})
          | SetPhaseReview(position) =>
            switch (state) {
            | PhaseGetGeolocation({data, objectUrl}) =>
              PhaseReview({
                coordinates: position->Geolocation.coordsGet,
                data,
                objectUrl,
              })
            | _ => PhaseError(`InvalidPhaseTransition)
            }
          | SetPhaseGetGeolocation(data) =>
            switch (state) {
            | PhaseRecording(_) =>
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
      let _ = setPhaseGetUserMedia->dispatchPhaseAction;
      let _ = onFile(~file=review.data, ~location=review.coordinates);
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
    <MediaRecorder_PhaseGetGeolocation onGranted=handleGrantedGeolocation />
  | PhaseRecording({stream}) =>
    <MediaRecorder_PhaseRecording
      stream
      onComplete=handleRecordingComplete
      onError=handleRecordingError
      mimeType
    />
  | PhaseReview({objectUrl}) =>
    let src =
      [|(objectUrl, mimeType)|]->VideoSurface.srcElement->Js.Option.some;
    <MediaRecorder_PhaseReview
      onApprove=handleReviewApprove
      onReject=handleReviewReject
      src
    />;
  | PhaseComplete(_) => React.null
  | PhaseError(_) => <Error />
  };
};
