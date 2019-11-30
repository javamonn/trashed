open Lib.Styles;

[@react.component]
let make = (~onPromptGeolocation, ~geolocationPermission) => {
  let handleClick = _ev => {
    let _ = onPromptGeolocation();
    ();
  };
  switch (geolocationPermission) {
  | Service.Permission.Unknown
  | Service.Permission.PermissionGranted(Some(_)) => React.null
  | _ =>
    <div
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
        className={cn(["mr-6", "flex-auto", "flex", "items-center", "pl-4"])}
        onClick=handleClick>
        <span
          className={cn([
            "text-black",
            "font-bold",
            "italic",
            "leading-none",
            "text-xl",
          ])}>
          {React.string(
             "trashed needs your location to show you nearby things.",
           )}
        </span>
      </Button>
      <Button
        className={cn([
          "flex",
          "justify-center",
          "items-center",
          "flex-shrink-0",
          "flex-auto",
          "h-24",
          "w-24"
        ])}
        onClick=handleClick>
        <GeolocationIconSVG className={cn(["h-20", "w-20"])} />
      </Button>
    </div>
  };
};
