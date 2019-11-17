open Lib.Styles;

let renderItem = (~itemId, ~isActive) =>
  <ScrollSnapList.Item key=itemId direction=ScrollSnapList.Vertical>
    <Container.Item key=itemId itemId autoPlay=isActive />
  </ScrollSnapList.Item>;

let renderPlaceholder = () =>
  <ScrollSnapList.Item direction=ScrollSnapList.Vertical />;

let renderContainer = (~onScroll, ~itemIdx, ~children) =>
  <ScrollSnapList.Container
    direction=ScrollSnapList.Vertical onScroll initialIdx=itemIdx>
    children
  </ScrollSnapList.Container>;

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
let make = (~itemId=?, ~nextToken=?, ~onReplaceUrlSearch) => {
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
