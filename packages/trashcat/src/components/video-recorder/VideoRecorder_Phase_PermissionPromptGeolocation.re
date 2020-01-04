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
           ? <div
               className={cn([
                 "relative",
                 "flex",
                 "justify-center",
                 "items-center",
               ])}>
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
           : <SVG
               className={cn(["h-56", "w-56"])}
               icon=SVG.geolocationIconOnBackground
               placeholderViewBox="0 0 224 224"
               onClick={_ => onClick()}
             />}
      </div>
    </div>;
};

[@react.component]
let make = (~onGranted, ~onPrompt, ~permission) => {
  let _ =
    React.useEffect1(
      () => {
        let _ =
          Service.Permission.(
            switch (permission) {
            | Unknown
            | Unprompted
            | PermissionGranted(None)
            | PermissionRejected
            | TransitionInProgress(_) => ()
            | PermissionGranted(Some(data)) => onGranted(data)
            }
          );
        None;
      },
      [|permission|],
    );

  let handlePrompt = () => {
    Js.log("on prompt");
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
      | PermissionGranted(Some(_)) => <Prompt isInProgress=true />
      | PermissionGranted(None)
      | Unprompted
      | PermissionRejected => <Prompt onClick=handlePrompt />
      }
    )
  </div>;
};
