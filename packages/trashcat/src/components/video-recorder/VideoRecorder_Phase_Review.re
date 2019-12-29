open Lib.Styles;
[@react.component]
let make = (~onApprove, ~onReject, ~src) => {
  <div
    className={cn(["w-full", "h-full", "relative", "overflow-hidden"])}>
    <ScreenBottomEdgeAligned
      className={cn([
        "flex",
        "flex-row",
        "justify-between",
        "my-8",
        "mx-8",
        "z-10",
      ])}>
      <div
        onTouchEnd={_ev => onReject()}
        className={cn(["w-12", "h-12", "p-1"])}>
        <SVG
          icon=SVG.emojiCrossMark
          className={cn(["w-full", "h-full"])}
          placeholderViewBox="0 0 128 128"
        />
      </div>
      <div onTouchEnd={_ev => onApprove()} className={cn(["w-12", "h-12"])}>
        <SVG
          icon=SVG.emojiWhiteCheckMark
          className={cn(["w-full", "h-full"])}
          placeholderViewBox="0 0 128 128"
        />
      </div>
    </ScreenBottomEdgeAligned>
    <VideoSurface src autoPlay=true />
  </div>;
};
