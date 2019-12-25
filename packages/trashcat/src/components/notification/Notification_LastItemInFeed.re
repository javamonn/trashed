open Lib.Styles;

[@react.component]
let make = (~_in, ~onClick) =>
  <Grow _in appear=true>
    <Notification_Base
      renderIcon={() =>
        <SVG
          placeholderViewBox="0 0 128 128"
          className={cn(["h-full", "w-full"])}
          icon=SVG.emojiRecycle
        />
      }
      renderText={() => React.string("no trash left in feed.")}
      onClick
    />
  </Grow>;
