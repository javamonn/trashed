open Lib.Styles;

module Prompt = {
  [@react.component]
  let make = (~onClick=() => (), ~isInProgress=false) =>
    <div className={cn(["w-full", "h-full", "flex", "flex-col"])}>
      <div
        className={cn([
          "flex-1",
          "flex-shrink-0",
          "bg-black",
          "flex",
          "justify-center",
          "items-center",
          "px-4",
        ])}>
        <span
          className={cn([
            "text-white",
            "font-bold",
            "italic",
            "text-4xl",
            "leading-none",
            "text-center",
          ])}>
          {React.string(
             "trashed needs access to your camera to take videos of things.",
           )}
        </span>
      </div>
      <div
        className={cn([
          "flex-1",
          "flex-shrink-0",
          "bg-brandBlue",
          "flex",
          "justify-center",
          "items-center",
        ])}>
        {isInProgress
           ? <div className={cn(["relative"])}>
               <SVG
                 className={cn(["h-56", "w-56"])}
                 icon=SVG.iconBackground
                 placeholderViewBox="0 0 192 192"
               />
               <div
                 className={cn([
                   "absolute",
                   "inset-0",
                   "flex",
                   "justify-center",
                   "items-center",
                 ])}>
                 <Progress />
               </div>
             </div>
           : <div onClick={_ => onClick()}>
               <SVG
                 className={cn(["h-56", "w-56"])}
                 icon=SVG.videoIconOnBackground
                 placeholderViewBox="0 0 224 224"
               />
             </div>}
      </div>
    </div>;
};

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
      "w-screen",
      "h-screen",
      "flex",
      "justify-center",
      "items-center",
    ])}>
    Service.Permission.(
      switch (permission) {
      | Unknown
      | TransitionInProgress(_)
      | PermissionGranted(Some(_)) => <Prompt isInProgress=true />
      | PermissionGranted(None)
      | Unprompted
      | PermissionRejected => <Prompt onClick=handlePrompt />
      }
    )
  </div>;
};
