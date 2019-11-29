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
    <div className={cn(["absolute", "inset-x-0", "top-0", "py-4", "px-6"])}>
      <Button className={cn(["mr-6"])} onClick=handleClick>
        {React.string(
           "trashed needs your location to show you nearby things.",
         )}
      </Button>
      <Button onClick=handleClick> <GeolocationIconSVG /> </Button>
    </div>
  };
};
