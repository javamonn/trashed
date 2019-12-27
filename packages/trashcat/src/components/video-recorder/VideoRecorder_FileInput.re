open Externals;
open VideoRecorder_Phase;

type error = [ | `InvalidPhaseTransition];

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
let make = (~mimeType, ~onFile) => {
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
          | _ => PhaseError(`InvalidPhaseTransition)
          }
        | SetPhaseReview(position) =>
          switch (state) {
          | PhaseGetGeolocation({data, objectUrl}) =>
            PhaseReview({data, objectUrl, position})
          | _ => PhaseError(`InvalidPhaseTransition)
          }
        | SetPhaseGetFile => PhaseGetFile
        | SetPhaseError(error) => PhaseError(error)
        },
      PhaseGetFile,
    );

  let handleGrantedGeolocation = position =>
    position->setPhaseReview->dispatchPhaseAction;

  let handleFile = file => file->setPhaseGetGeolocation->dispatchPhaseAction;

  let handleReviewApprove = () =>
    switch (phaseState) {
    | PhaseReview({data, position}) =>
      let _ = setPhaseGetFile->dispatchPhaseAction;
      let _ = onFile(~file=data, ~location=position->Geolocation.coordsGet);
      ();
    | _ =>
      let _ = `InvalidPhaseTransition->setPhaseError->dispatchPhaseAction;
      ();
    };

  let handleReviewReject = () =>
    switch (phaseState) {
    | PhaseReview(_) =>
      let _ = setPhaseGetFile->dispatchPhaseAction;
      ();
    | _ =>
      let _ = `InvalidPhaseTransition->setPhaseError->dispatchPhaseAction;
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
    let src = [|(objectUrl, mimeType)|]->VideoSurface.srcElement;
    <Review
      onApprove=handleReviewApprove
      onReject=handleReviewReject
      src
    />
  | PhaseError(_) => <Error />
  };
};
