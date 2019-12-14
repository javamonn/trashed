let imageSize = 130.;

let mapsUrl = (~lat, ~lon) => {j|https://www.google.com/maps/search/?api=1&query=$lat,$lon|j};

let imgUrl = (~lat, ~lon) => {
  let accessToken = "pk.eyJ1IjoiamF2YW1vbm4iLCJhIjoiY2szYzJwNm03MGhuNTNscGlsemUwajE0ayJ9.tdGviHSB0JAHVtWWT2Ezww";
  let pinColor = "000";

  {j|https://api.mapbox.com/styles/v1/mapbox/streets-v11/static/pin-s+$pinColor($lon,$lat)/$lon,$lat,12/200x200@2x?access_token=$accessToken|j};
};

[@react.component]
let make = (~className=?, ~style=?, ~location as (lat, lon)) =>
  <a
    ?style
    className={Cn.make(["relative", className->Cn.unpack])}
    href={mapsUrl(~lat, ~lon)}>
    <div
      className={Cn.make([
        "bg-black",
        "absolute",
        "overflow-hidden",
        "rounded-full",
      ])}
      style=Lib.Styles.(
        style(
          ~width=px(imageSize),
          ~height=px(imageSize),
          ~top=px(2.),
          ~left=px(2.),
          ~zIndex="-1",
          (),
        )
      )
    />
    {ReactDOMRe.createElementVariadic(
       "img",
       ~props=
         ReactDOMRe.objToDOMProps({
           "className": Cn.make(["overflow-hidden", "rounded-full"]),
           "style":
             Lib.Styles.style(
               ~width=Lib.Styles.px(imageSize),
               ~height=Lib.Styles.px(imageSize),
               ~maxWidth="unset",
               (),
             ),
           "src": imgUrl(~lat, ~lon),
           "crossorigin": "anonymous",
         }),
       [||],
     )}
  </a>;
