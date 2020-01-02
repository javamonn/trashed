open Lib.Styles;

[@react.component]
let make = (~onClick=() => (), ~renderButton=?, ~isInProgress=false) =>
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
      {switch (renderButton) {
       | _ when isInProgress =>
         <div className={cn(["relative"])}>
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
       | Some(renderButton) => renderButton()
       | None =>
         <div onClick={_ => onClick()}>
           <SVG
             className={cn(["h-56", "w-56"])}
             icon=SVG.videoIconOnBackground
             placeholderViewBox="0 0 224 224"
           />
         </div>
       }}
    </div>
  </div>;
