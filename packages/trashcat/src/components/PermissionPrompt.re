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

let defaultRenderPendingSystemPrompt = () => <Progress />;
let defaultRenderUnknownPermissionStatus = () => <Progress />;

[@react.component]
let make =
    (
      ~onPrompt,
      ~renderPrompt,
      ~renderPendingSystemPrompt=defaultRenderPendingSystemPrompt,
      ~renderUnknownPermissionStatus=defaultRenderUnknownPermissionStatus,
      ~onPermissionGranted,
      ~permission,
    ) => {
  let (phase, dispatch) =
    React.useReducer(
      (_phase, action) =>
        switch (action) {
        | SetPhase(p) => p
        },
      UnknownPermissionStatus,
    );

  let _ =
    React.useEffect1(
      () => {
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
      },
      [|permission|],
    );

  let _ =
    React.useEffect2(
      () => {
        let _ =
          switch (phase) {
          | PermissionGranted => onPermissionGranted()
          | _ => ()
          };
        None;
      },
      (phase, permission),
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

  switch (phase) {
  | UnknownPermissionStatus => renderUnknownPermissionStatus()
  | PendingInternalPrompt => renderPrompt(~onClick=handleClick)
  | PendingSystemPrompt => renderPendingSystemPrompt()
  | PermissionGranted => <div> {React.string("Permission granted.")} </div>
  | PermissionRejected => <div> {React.string("Permission rejected.")} </div>
  };
};

module Basic = {
  [@react.component]
  let make = (~text, ~onClick) =>
    <div className={cn(["max-w-sm", "px-5"])}>
      <Typography.PrimaryText className={cn(["block", "mb-4"])}>
        {React.string(text)}
      </Typography.PrimaryText>
      <Button onClick> {React.string("OK")} </Button>
    </div>;
};
