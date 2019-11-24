open Lib.Styles;
[@react.component]
let make = (~onApprove, ~onReject, ~src) => {
  <div
    className={cn(["w-screen", "h-screen", "relative", "overflow-hidden"])}>
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
      <div
        onTouchEnd={_ev => onReject()}
        className={cn(["w-12", "h-12", "p-1"])}>
        <Emoji_CrossMarkSVG className={cn(["w-fill", "h-fill"])} />
      </div>
      <div onTouchEnd={_ev => onApprove()} className={cn(["w-12", "h-12"])}>
        <Emoji_WhiteCheckMarkSVG className={cn(["w-fill", "h-fill"])} />
      </div>
    </div>
    <VideoSurface ?src autoPlay=true />
  </div>;
};
