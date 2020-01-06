open Lib.Styles;

[@react.component]
let make = (~onFile) => {
  let onFileInputChange = ev => {
    let _ = ReactEvent.Form.target(ev)##files->Belt.Array.getExn(0)->onFile;
    ();
  };

  /** On prompt, browser chrome shrinks, but grow event is not fired upon return to browser context. **/
  let _ =
    React.useEffect0(() => {
      Some(() => Service.RootHeightManager.calculateRootHeight())
    });

  <div
    className={cn([
      "w-full",
      "h-full",
      "flex",
      "justify-center",
      "items-center",
    ])}>
    <VideoRecorder_Phase_PermissionPromptCamera_Prompt
      renderButton={() =>
        <div className={cn(["h-56", "w-56", "relative"])}>
          <SVG
            className={cn(["inset-x-0", "inset-y-0", "absolute"])}
            icon=SVG.videoIconOnBackground
            placeholderViewBox="0 0 224 224"
          />
          {ReactDOMRe.createElementVariadic(
             "input",
             ~props=
               ReactDOMRe.objToDOMProps({
                 "type": "file",
                 "accept": "video/*",
                 "onChange": onFileInputChange,
                 "capture": "environment",
                 "className":
                   cn(["absolute", "inset-x-0", "inset-y-0", "opacity-0"]),
               }),
             [||],
           )}
        </div>
      }
    />
  </div>;
};
