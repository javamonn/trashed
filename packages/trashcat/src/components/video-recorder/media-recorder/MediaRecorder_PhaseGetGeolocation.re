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
             "trashed needs access to your location to know where things are.",
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
               <IconBackgroundSVG
                 className={cn(["h-56", "w-56"])}
                 backgroundColor=Colors.brandYellow
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
               <GeolocationIconOnBackgroundSVG
                 className={cn(["h-56", "w-56"])}
                 backgroundColor=Colors.brandYellow
               />
             </div>}
      </div>
    </div>;
};

[@react.component]
let make = (~onGranted) => {
  let (permission, onPrompt, _) = Service.Permission.Geolocation.use();

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
          | PermissionGranted(Some(data)) => onGranted(data)
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
