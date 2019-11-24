open Lib.Styles;

[@react.component]
let make = (~itemId=?, ~nextToken=?, ~onReplaceUrlSearch, ~isActive) => {
  let renderItem = (~itemId, ~isActive as isItemActive) =>
    <ScrollSnapList.Item key=itemId direction=ScrollSnapList.Horizontal>
      <Container.Item key=itemId itemId autoPlay={isActive && isItemActive} />
    </ScrollSnapList.Item>;

  let renderPlaceholder = () =>
    <ScrollSnapList.Item direction=ScrollSnapList.Horizontal />;

  let renderContainer = (~onScroll, ~itemIdx, ~item, ~children) =>
    <>
      <ScrollSnapList.Container
        direction=ScrollSnapList.Horizontal onScroll initialIdx=itemIdx>
        children
      </ScrollSnapList.Container>
      <ScreenBottomEdgeAligned className={cn(["flex", "justify-end"])}>
        <Map
          className={cn([
            "block",
            "overflow-hidden",
            "rounded-tl-full",
            "w-40",
            "h-40",
            "border-black",
            "border-t-4",
            "border-l-4",
          ])}
          location=(item##location##lat, item##location##lon)
        />
      </ScreenBottomEdgeAligned>
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
  />;
};
