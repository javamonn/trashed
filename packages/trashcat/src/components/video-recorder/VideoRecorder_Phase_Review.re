open Lib.Styles;
[@react.component]
let make = (~onApprove, ~onReject, ~src) => {
  <div className={cn(["w-full", "h-full", "relative", "overflow-hidden"])}>
    <div
      className={cn([
        "absolute",
        "inset-x-0",
        "bottom-0",
        "flex",
        "flex-row",
        "justify-between",
        "my-8",
        "mx-8",
        "z-10",
      ])}>
      <SVG
        icon=SVG.emojiCrossMark
        onClick={_ev => onReject()}
        className={cn(["w-12", "h-12"])}
        placeholderViewBox="0 0 128 128"
      />
      <SVG
        onClick={_ev => onApprove()}
        icon=SVG.emojiWhiteCheckMark
        className={cn(["w-12", "h-12"])}
        placeholderViewBox="0 0 128 128"
      />
    </div>
    <VideoSurface src autoPlay=true />
  </div>;
};
