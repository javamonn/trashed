open Lib.Styles;
open Externals;

let renderItem = (~itemId, ~isActive) =>
  <Container.Item
    key=itemId
    itemId
    autoPlay={isActive}
    style={
      style()
      ->unsafeStyle([
          ("scrollSnapStop", "always"),
          ("scrollSnapAlign", "start"),
        ])
    }
  />;

let renderPlaceholder = () =>
  <div
    style={
      style()
      ->unsafeStyle([
          ("scrollSnapStop", "always"),
          ("scrollSnapAlign", "start"),
        ])
    }
    className={cn(["w-screen", "h-screen"])}
  />;

let renderContainer = (~onScroll, ~children) =>
  <div
    onScroll
    className={cn(["w-screen", "h-screen", "overflow-y-scroll"])}
    style={
      style()
      ->unsafeStyle([
          ("scrollSnapType", "y mandatory"),
          ("scrollSnapStop", "always"),
        ])
    }>
    children
  </div>;

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

[@react.component]
let make = (~itemId=?, ~nextToken=?) => {
  <Container.Items
    renderItem
    renderPlaceholder
    renderContainer
    renderLoading
    renderError
    onChange={(~nextToken, ~itemId, ()) => {
      let newSearch =
        [|("nextToken", nextToken), ("itemId", itemId)|]
        ->Belt.Array.keepMap(((key, value)) =>
            value->Belt.Option.map(value => (key, value))
          )
        ->Webapi.Url.URLSearchParams.makeWithArray
        ->Webapi.Url.URLSearchParams.toString;
      let _ = ReasonReactRouter.replace("/item?" ++ newSearch);
      ();
    }}
    ?nextToken
    ?itemId
  />;
};
