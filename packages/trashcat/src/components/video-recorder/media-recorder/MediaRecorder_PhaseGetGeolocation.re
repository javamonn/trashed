open Lib.Styles;

module Prompt = {
  [@react.component]
  let make = (~onClick=?, ~isInProgress=false) =>
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
           : <div ?onClick>
               <GeolocationIconOnBackgroundSVG
                 className={cn(["h-56", "w-56"])}
                 backgroundColor=Colors.brandYellow
               />
             </div>}
      </div>
    </div>;
};

[@react.component]
let make = (~onGetGeolocation, ~onGetGeolocationGranted) => {
  <div
    className={cn([
      "w-screen",
      "h-screen",
      "flex",
      "justify-center",
      "items-center",
    ])}>
    <PermissionPrompt
      renderPrompt={(~onClick) => <Prompt onClick />}
      renderPendingSystemPrompt={() => <Prompt isInProgress=true />}
      renderUnknownPermissionStatus={() =>
        <div
          className={cn([
            "w-full",
            "h-full",
            "bg-brandBlue",
            "flex",
            "justify-center",
            "items-center",
          ])}>
          <Progress />
        </div>
      }
      onPrompt=onGetGeolocation
      onPermissionGranted=onGetGeolocationGranted
      permission=`Geolocation
    />
  </div>;
};
