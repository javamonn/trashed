open Externals;
open Lib.Styles;

type phase =
  | UnknownPermissionStatus
  | PendingInternalPrompt
  | PendingSystemPrompt
  | PermissionGranted
  | PermissionRejected;

[@bs.deriving accessors]
type action =
  | SetPhase(phase);

[@react.component]
let make = (~onPrompt, ~renderPromptText, ~onPermissionGranted, ~permission) => {
  let (phase, dispatch) =
    React.useReducer(
      (_phase, action) =>
        switch (action) {
        | SetPhase(p) => p
        },
      UnknownPermissionStatus,
    );

  let _ =
    React.useEffect0(() => {
      let _ =
        Permissions.(query(Descriptor.make(~name=permission)))
        |> Js.Promise.then_(s => {
             let phase =
               switch (s->Belt.Option.map(Permissions.Status.stateGet)) {
               | Some(`Prompt) => PendingInternalPrompt
               | Some(`Granted) => PermissionGranted
               | Some(`Denied) => PermissionRejected
               | None => PendingInternalPrompt
               };
             let _ = phase->setPhase->dispatch;
             Js.Promise.resolve();
           });
      None;
    });

  let _ =
    React.useEffect1(
      () => {
        let _ =
          switch (phase) {
          | PermissionGranted => onPermissionGranted()
          | _ => ()
          };
        None;
      },
      [|phase|],
    );

  let handleClick = _ev => {
    let _ = PendingSystemPrompt->setPhase->dispatch;
    let _ =
      onPrompt()
      |> Js.Promise.then_(s => {
           let phase =
             switch (s->Belt.Option.map(Permissions.Status.stateGet)) {
             | Some(`Granted) => PermissionGranted
             | Some(`Denied) => PermissionRejected
             | _ => phase
             };
           let _ = phase->setPhase->dispatch;
           Js.Promise.resolve();
         });
    ();
  };

  <div className={cn(["flex", "justify-center", "align-center"])}>
    {switch (phase) {
     | UnknownPermissionStatus => <Progress />
     | PendingInternalPrompt =>
       <div>
         {renderPromptText()}
         <button type_="button" onClick=handleClick>
           {React.string("OK")}
         </button>
       </div>
     | PendingSystemPrompt => <Progress />
     | PermissionGranted => <div> {React.string("Permission granted.")} </div>
     | PermissionRejected =>
       <div> {React.string("Permission rejected.")} </div>
     }}
  </div>;
};
