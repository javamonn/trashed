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

[@react.component]
let make = (~className=?, ~placeholderClassName=?, ~placeholderViewBox, ~icon) => {
  let (isLoading, setIsLoading) = React.useState(() => true);

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
      onLoad={_ => setIsLoading(_ => false)}
      type_="image/svg+xml"
      className={
        isLoading
          ? cn(["invisible", "absolute", "w-0", "h-0"]) : className->Cn.unpack
      }
      data=icon
    />
  </>;
};
