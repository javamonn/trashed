open Lib.Styles;

let mapsUrl = (~lat, ~lon) => {j|https://www.google.com/maps/search/?api=1&query=$lat,$lon|j};

[@react.component]
let make = (~className=?, ~location as (lat, lon)) =>
  <a ?className href={mapsUrl(~lat, ~lon)}>
    <GeolocationIconOnBackgroundSVG
      className={cn(["h-24", "w-24"])}
      backgroundColor=Colors.brandYellow
    />
  </a>;
