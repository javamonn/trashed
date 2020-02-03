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
let emojiRecycle: string = [%raw "require('./emoji-recycle.svg')"];
let emojiWarning: string = [%raw "require('./emoji-warning.svg')"];

type state = {
  isLoading: bool,
  src: string,
};

[@react.component]
let make =
    (
      ~className=?,
      ~placeholderClassName=?,
      ~onClick=?,
      ~placeholderViewBox,
      ~icon,
    ) => {
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
      className={
        placeholderClassName->Belt.Option.getWithDefault(
          cn(["w-full", "h-full"]),
        )
      }
      viewBox=placeholderViewBox
      fill="none"
      xmlns="http://www.w3.org/2000/svg"
    />;

  let base =
    <>
      {isLoading ? renderPlaceholder() : React.null}
      <object
        onLoad={_ => setState(_ => {isLoading: false, src})}
        type_="image/svg+xml"
        className={
          isLoading
            ? cn(["invisible", "absolute", "w-0", "h-0", "relative"])
            : cn([className->Cn.unpack])
        }
        data=src
      />
    </>;
  switch (onClick) {
  | Some(onClick) =>
    <div className={cn([className->Cn.unpack, "relative"])}>
      base
      <div
        className={cn(["absolute", "inset-x-0", "inset-y-0"])}
        onClick={_ => onClick()}
      />
    </div>
  | None => base
  };
};
