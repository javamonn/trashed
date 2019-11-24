open Lib.Styles;

let imageSize = 200.;

let mapsUrl = (~lat, ~lon) => {j|https://www.google.com/maps/search/?api=1&query=$lat,$lon|j};

let imgUrl = (~lat, ~lon) => {
  let accessToken = "pk.eyJ1IjoiamF2YW1vbm4iLCJhIjoiY2szYzJwNm03MGhuNTNscGlsemUwajE0ayJ9.tdGviHSB0JAHVtWWT2Ezww";
  let pinColor = "000";

  {j|https://api.mapbox.com/styles/v1/mapbox/streets-v11/static/pin-s+$pinColor($lon,$lat)/$lon,$lat,12/200x200@2x?access_token=$accessToken|j};
};

[@react.component]
let make = (~className=?, ~location as (lat, lon)) =>
  <a ?className href={mapsUrl(~lat, ~lon)}>
    <img
      style={style(
        ~width=px(imageSize),
        ~height=px(imageSize),
        ~maxWidth="unset",
        (),
      )}
      src={imgUrl(~lat, ~lon)}
    />
  </a>;
