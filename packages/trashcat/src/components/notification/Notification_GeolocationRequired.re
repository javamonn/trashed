open Lib.Styles;

module GeolocationRequired = {
  [@react.component]
  let make = (~permission, ~onClick, ~style=?) =>
    <div
      ?style
      className={cn([
        "absolute",
        "inset-x-0",
        "top-0",
        "h-24",
        "my-4",
        "mx-6",
        "z-10",
        "flex",
        "flex-row",
      ])}>
      <Button
        className={cn([
          "flex-auto",
          "flex",
          "items-center",
          "justify-between",
        ])}
        onClick>
        <div
          className={cn([
            "h-24",
            "w-24",
            "flex-shrink-0",
            "flex",
            "flex-auto",
            "items-center",
            "justify-center",
            "p-6",
          ])}>
          {switch (permission) {
           | Service.Permission.TransitionInProgress(_) =>
             <Progress className={cn(["w-full"])} />
           | _ =>
             <SVG
               placeholderViewBox="0 0 224 224"
               className={cn(["h-full", "w-full"])}
               icon=SVG.geolocation
             />
           }}
        </div>
        <div
          className={cn([
            "text-black",
            "font-bold",
            "italic",
            "leading-none",
            "text-xl",
            "mr-6",
          ])}>
          {React.string(
             "trashed needs your location to show you nearby things.",
           )}
        </div>
      </Button>
    </div>;
};

[@react.component]
let make = (~_in, ~permission, ~onClick) =>
  <Grow _in appear=true> <GeolocationRequired permission onClick /> </Grow>;
