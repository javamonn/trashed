open Externals;
open Lib.Styles;
open Lib.Utils;

module HtmlInputElement = {
  type t = Webapi.Dom.HtmlInputElement.t;
  external ofHtmlElement: Webapi.Dom.HtmlElement.t => t = "%identity";
};

module PhaseState = {
  module GetGeolocation = {
    [@bs.deriving abstract]
    type t = {data: File.t};
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

  [@bs.deriving accessors]
  type t =
    | PhaseGetFile
    | PhaseGetGeolocation(GetGeolocation.t)
    | PhaseComplete(Complete.t);

  [@bs.deriving accessors]
  type action =
    | SetPhase(t);
};

[@react.component]
let make = (~mimeType, ~onFile) => {
  let inputRef = React.useRef(Js.Nullable.null);
  let (phaseState, dispatchPhaseAction) =
    React.useReducer(
      (state, action) =>
        switch (action) {
        | PhaseState.SetPhase(p) => p
        },
      PhaseState.PhaseGetFile,
    );

  let proxyClick = () => {
    let _ =
      inputRef
      ->React.Ref.current
      ->Js.Nullable.toOption
      ->Belt.Option.flatMap(Webapi.Dom.HtmlElement.ofElement)
      ->Belt.Option.map(Webapi.Dom.HtmlElement.click);
    ();
  };
  let _ =
    React.useEffect0(() => {
      proxyClick();
      None;
    });

  let handleChange = _ev => {
    inputRef
    ->React.Ref.current
    ->Js.Nullable.toOption
    ->Belt.Option.map(elem => {
        let _ =
          elem
          ->Webapi.Dom.HtmlElement.ofElement
          ->Belt.Option.map(HtmlInputElement.ofHtmlElement)
          ->Belt.Option.map(Webapi.Dom.HtmlInputElement.value)
          ->Belt.Option.map(File.stringToFile)
          ->Belt.Option.map(f =>
              PhaseState.(
                GetGeolocation.make(~data=f)
                ->phaseGetGeolocation
                ->setPhase
                ->dispatchPhaseAction
              )
            );
        ();
      });
  };

  let handleGetGeolocation = () =>
    Js.Promise.make((~resolve, ~reject) =>
      Geolocation.getCurrentPosition(
        position => {
          let _ =
            PhaseState.(
              switch (phaseState) {
              | PhaseGetGeolocation(getGeolocation) =>
                let _ =
                  Complete.make(
                    ~coordinates=position->Geolocation.coordsGet,
                    ~data=getGeolocation->GetGeolocation.dataGet,
                  )
                  ->phaseComplete
                  ->setPhase
                  ->dispatchPhaseAction;
                let _ =
                  onFile(
                    ~file=getGeolocation->GetGeolocation.dataGet,
                    ~location=position->Geolocation.coordsGet,
                  );
                ();
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
  let handleGetGeolocationGranted = () => {
    switch (phaseState) {
    | PhaseGetGeolocation(_) =>
      let _ = handleGetGeolocation();
      ();
    | _ => ()
    };
  };

  switch (phaseState) {
  | PhaseState.PhaseGetFile =>
    let input =
      ReactDOMRe.createElementVariadic(
        "input",
        ~props=
          ReactDOMRe.objToDOMProps({
            "type": "file",
            "accept": "video/*",
            "onChange": handleChange,
            "capture": "environment",
            "ref": inputRef->ReactDOMRe.Ref.domRef,
          }),
        [||],
      );
    <div
      onClick={_ev => proxyClick()}
      className={cn(["w-screen", "h-screen", "opacity-0"])}>
      input
    </div>;
  | PhaseState.PhaseGetGeolocation(_) =>
    <div
      className={cn([
        "w-screen",
        "h-screen",
        "flex",
        "justify-center",
        "align-center",
        "flex-col",
      ])}>
      <PermissionPrompt
        renderPrompt={(~onClick) =>
          <PermissionPrompt.Basic
            text="Trashed needs to access your location to know where things (and you) are."
            onClick
          />
        }
        onPrompt=handleGetGeolocation
        onPermissionGranted=handleGetGeolocationGranted
        permission=`Geolocation
      />
    </div>
  | PhaseState.PhaseComplete(_) => React.null
  };
};
