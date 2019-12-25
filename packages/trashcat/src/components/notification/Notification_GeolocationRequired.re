open Lib.Styles;

[@react.component]
let make = (~_in, ~permission, ~onClick) =>
  <Grow _in appear=true>
    <Notification_Base
      onClick
      renderIcon={() =>
        switch (permission) {
        | Service.Permission.TransitionInProgress(_) =>
          <Progress className={cn(["w-full"])} />
        | _ =>
          <SVG
            placeholderViewBox="0 0 224 224"
            className={cn(["h-full", "w-full"])}
            icon=SVG.geolocation
          />
        }
      }
      renderText={() =>
        React.string("trashed needs your location to show you nearby things.")
      }
    />
  </Grow>;
