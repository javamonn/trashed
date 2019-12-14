open Lib.Styles;

let geolocation: string = [%raw "require('./geolocation.svg')"];
let trashFavorite: string = [%raw "require('./trash-favorite.svg')"];
let videoIconOnBackground: string = [%raw
  "require('./video-icon-on-background.svg')"
];
let geolocationIconOnBackground: string = [%raw
  "require('./geolocation-icon-on-background.svg')"
];
let trashIconOnBackground: string = [%raw
  "require('./trash-icon-on-background.svg')"
];
let iconBackground: string = [%raw "require('./icon-background.svg')"];

let trashedText: string = [%raw "require('./trashed-text.svg')"];

let emojiCrossMark: string = [%raw "require('./emoji-cross-mark.svg')"];

let emojiWhiteCheckMark: string = [%raw
  "require('./emoji-white-check-mark.svg')"
];

let emojiHourglass: string = [%raw "require('./emoji-hourglass.svg')"];

type state = {
  isLoading: bool,
  src: string,
};

[@react.component]
let make = (~className=?, ~placeholderClassName=?, ~placeholderViewBox, ~icon) => {
  let ({isLoading, src}, setState) =
    React.useState(() => {isLoading: true, src: icon});

  let _ =
    React.useEffect1(
      () => {
        if (src !== icon) {
          let _ = setState(_ => {isLoading: true, src: icon});
          ();
        };
        None;
      },
      [|icon|],
    );

  let renderPlaceholder = () =>
    <svg
      className={cn(["w-full", "h-full", placeholderClassName->Cn.unpack])}
      viewBox=placeholderViewBox
      fill="none"
      xmlns="http://www.w3.org/2000/svg"
    />;

  <>
    {isLoading ? renderPlaceholder() : React.null}
    <object
      onLoad={_ => setState(_ => {isLoading: false, src})}
      type_="image/svg+xml"
      className={
        isLoading
          ? cn([
              "invisible",
              "absolute",
              "w-0",
              "h-0",
              "pointer-events-none",
            ])
          : cn(["pointer-events-none", className->Cn.unpack])
      }
      data=src
    />
  </>;
};
