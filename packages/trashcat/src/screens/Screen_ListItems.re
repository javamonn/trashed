open Lib.Styles;
open Externals;

[@react.component]
let make = (~itemId=?, ~nextToken=?, ~onReplaceUrlSearch, ~isActive) => {
  let (geolocationPermission, onPromptGeolocation, _) =
    Service.Permission.Geolocation.use();
  let location =
    switch (geolocationPermission) {
    | Service.Permission.PermissionGranted(Some(pos)) =>
      Some({
        "lat": pos->Geolocation.coordsGet->Geolocation.latitudeGet,
        "lon": pos->Geolocation.coordsGet->Geolocation.longitudeGet,
      })
    | _ => None
    };

  let renderItem = (~itemId, ~isActive as isItemActive) =>
    <ScrollSnapList.Item key=itemId direction=ScrollSnapList.Horizontal>
      <Container.Item key=itemId itemId autoPlay={isActive && isItemActive} />
    </ScrollSnapList.Item>;

  let renderPlaceholder = () =>
    <ScrollSnapList.Item direction=ScrollSnapList.Horizontal />;

  let renderContainer = (~onScroll, ~itemIdx, ~item, ~children) =>
    <>
      <ItemTopOverlay onPromptGeolocation geolocationPermission />
      <ScrollSnapList.Container
        direction=ScrollSnapList.Horizontal onScroll initialIdx=itemIdx>
        children
      </ScrollSnapList.Container>
      <ItemBottomOverlay item />
    </>;

  let renderLoading = () =>
    <div
      className={cn([
        "w-screen",
        "h-screen",
        "flex",
        "justify-center",
        "items-center",
      ])}>
      <Progress />
    </div>;

  let renderError = () => <Error />;

  <Container.Items
    renderItem
    renderPlaceholder
    renderContainer
    renderLoading
    renderError
    onChange={(~nextToken, ~itemId, ()) => {
      let _ =
        [|("nextToken", nextToken), ("itemId", itemId)|]
        ->Belt.Array.keepMap(((key, value)) =>
            value->Belt.Option.map(value => (key, value))
          )
        ->Webapi.Url.URLSearchParams.makeWithArray
        ->Webapi.Url.URLSearchParams.toString
        ->onReplaceUrlSearch;
      ();
    }}
    ?nextToken
    ?itemId
    location={
      location->Belt.Option.getWithDefault({"lat": 40.657, "lon": (-73.956)})
    }
  />;
};
