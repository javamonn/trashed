open Externals;
open VideoRecorder_Phase;

[@bs.deriving jsConverter]
type error = [
  | [@bs.as "InvalidPhaseTransitionAttempt_GetGeolocation"]
    `InvalidPhaseTransitionAttempt_GetGeolocation
  | [@bs.as "InvalidPhaseTransitionAttempt_Review"]
    `InvalidPhaseTransitionAttempt_Review
  | [@bs.as "InvalidPhaseTransitionAttempt_Approve"]
    `InvalidPhaseTransitionAttempt_Approve
  | [@bs.as "InvalidPhaseTransitionAttempt_Reject"]
    `InvalidPhaseTransitionAttempt_Reject
];

type geolocation = {
  data: File.t,
  objectUrl: string,
};
type review = {
  position: Geolocation.position,
  data: File.t,
  objectUrl: string,
};

[@bs.deriving accessors]
type t =
  | PhaseGetFile
  | PhaseGetGeolocation(geolocation)
  | PhaseReview(review)
  | PhaseError(error);

[@bs.deriving accessors]
type action =
  | SetPhase(t)
  | SetPhaseGetFile
  | SetPhaseGetGeolocation(File.t)
  | SetPhaseReview(Geolocation.position)
  | SetPhaseError(error);

[@react.component]
let make = (~onFile) => {
  let (geolocationPermission, onGeolocationPrompt, _) =
    Service.Permission.Geolocation.use();

  let (phaseState, dispatchPhaseAction) =
    React.useReducer(
      (state, action) =>
        switch (action) {
        | SetPhase(p) => p
        | SetPhaseGetGeolocation(data) =>
          switch (state) {
          | PhaseGetFile =>
            PhaseGetGeolocation({
              data,
              objectUrl: Webapi.Url.createObjectURL(data),
            })
          | _ => PhaseError(`InvalidPhaseTransitionAttempt_GetGeolocation)
          }
        | SetPhaseReview(position) =>
          switch (state) {
          | PhaseGetGeolocation({data, objectUrl}) =>
            PhaseReview({data, objectUrl, position})
          | _ => PhaseError(`InvalidPhaseTransitionAttempt_Review)
          }
        | SetPhaseGetFile => PhaseGetFile
        | SetPhaseError(error) => PhaseError(error)
        },
      PhaseGetFile,
    );

  let handleGrantedGeolocation = position =>
    position->setPhaseReview->dispatchPhaseAction;

  let handleFile = file => {
    let _ = file->setPhaseGetGeolocation->dispatchPhaseAction;
    ();
  };

  let handleReviewApprove = () =>
    switch (phaseState) {
    | PhaseReview({data, position}) =>
      let _ = setPhaseGetFile->dispatchPhaseAction;
      let _ = onFile(~file=data, ~location=position->Geolocation.coordsGet);
      ();
    | _ =>
      let _ =
        `InvalidPhaseTransitionAttempt_Approve
        ->setPhaseError
        ->dispatchPhaseAction;
      ();
    };

  let handleReviewReject = () =>
    switch (phaseState) {
    | PhaseReview(_) =>
      let _ = setPhaseGetFile->dispatchPhaseAction;
      ();
    | _ =>
      let _ =
        `InvalidPhaseTransitionAttempt_Reject
        ->setPhaseError
        ->dispatchPhaseAction;
      ();
    };

  switch (phaseState) {
  | PhaseGetFile => <PermissionPromptCamera.FileInput onFile=handleFile />
  | PhaseGetGeolocation(_) =>
    <PermissionPromptGeolocation
      permission=geolocationPermission
      onPrompt=onGeolocationPrompt
      onGranted=handleGrantedGeolocation
    />
  | PhaseReview({objectUrl}) =>
    let src = objectUrl->VideoSurface.srcUrl;
    <Review onApprove=handleReviewApprove onReject=handleReviewReject src />;
  | PhaseError(error) => error->errorToJs->Js.Exn.raiseError
  };
};
