open Lib.Styles;

[@react.component]
let make = (~_in) =>
  <Grow _in appear=true>
    <Notification_Base
      renderIcon={() => 
        <SVG
          className={cn(["h-full", "w-full"])}
          icon=SVG.trashFavorite
          placeholderViewBox="0 0 57 62"
        />
      }
      renderText={() => React.string("trash was posted!")}
    />
  </Grow>;
