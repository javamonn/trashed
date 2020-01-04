open Lib.Styles;

[@react.component]
let make = (~onGranted) => {
  let (permission, onPrompt, setStatus) = Service.Permission.Camera.use();

  let _ =
    React.useEffect1(
      () => {
        let _ =
          switch (permission) {
          | Unknown
          | Unprompted
          | PermissionGranted(None)
          | PermissionRejected
          | TransitionInProgress(_) => ()
          | PermissionGranted(Some(data)) =>
            let _ = onGranted(data);
            let _ = setStatus(Service.Permission.PermissionGranted(None));
            ();
          };
        None;
      },
      [|permission|],
    );

  let handlePrompt = () => {
    let _ = onPrompt();
    ();
  };

  <div
    className={cn([
      "w-full",
      "h-full",
      "flex",
      "justify-center",
      "items-center",
    ])}>
    Service.Permission.(
      switch (permission) {
      | Unknown
      | TransitionInProgress(_)
      | PermissionGranted(Some(_)) => <VideoRecorder_Phase_PermissionPromptCamera_Prompt isInProgress=true />
      | PermissionGranted(None)
      | Unprompted
      | PermissionRejected => <VideoRecorder_Phase_PermissionPromptCamera_Prompt onClick=handlePrompt />
      }
    )
  </div>;
};
