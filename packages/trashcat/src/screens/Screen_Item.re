open Lib.Styles;

module Tab = {
  type t =
    | New
    | Feed
    | Search;

  let ordered = () => [|New, Feed, Search|];

  let indexOf =
    fun
    | New => 0
    | Feed => 1
    | Search => 2;

  let pathOf =
    fun
    | New => "new"
    | Feed => "feed"
    | Search => "search";

  let fromPath =
    fun
    | "new" => Some(New)
    | "feed" => Some(Feed)
    | "search" => Some(Search)
    | _ => None;

  let equal = (t1, t2) => pathOf(t1) === pathOf(t2);
};

[@react.component]
let make = (~tab, ~url) => {
  let params = Webapi.Url.URLSearchParams.make(url.ReasonReactRouter.search);
  let nextToken = Webapi.Url.URLSearchParams.get("nextToken", params);
  let itemId = Webapi.Url.URLSearchParams.get("itemId", params);

  let handleIdxChange = idx => {
    let tabs = Tab.ordered();

    let newTab = tabs[idx];
    let pathArr = Array.of_list(url.path);
    if (newTab !== tab) {
      let _ =
        Belt.Array.(set(pathArr, length(pathArr) - 1, Tab.pathOf(newTab)));
      let _ =
        pathArr
        |> Js.Array.joinWith("/")
        |> (path => "/" ++ path ++ "?" ++ url.search)
        |> ReasonReactRouter.replace;
      ();
    };
    ();
  };

  let handleVisibleItemChange = (~nextToken, ~itemId, ()) => {
    let newSearch =
      [|("nextToken", nextToken), ("itemId", itemId)|]
      ->Belt.Array.keepMap(((key, value)) =>
          value->Belt.Option.map(value => (key, value))
        )
      ->Webapi.Url.URLSearchParams.makeWithArray
      ->Webapi.Url.URLSearchParams.toString;
    let _ =
      url.path
      |> Array.of_list
      |> Js.Array.joinWith("/")
      |> (path => "/" ++ path ++ "?" ++ newSearch)
      |> ReasonReactRouter.replace;
    ();
  };

  <ScrollSnapList.Container
    direction=ScrollSnapList.Horizontal
    initialIdx={Tab.indexOf(tab)}
    onIdxChange=handleIdxChange>
    [|
      <ScrollSnapList.Item direction=ScrollSnapList.Horizontal>
        <Container.ItemNew isActive={Tab.equal(tab, New)} />
      </ScrollSnapList.Item>,
      <ScrollSnapList.Item
        className={cn(["relative"])} direction=ScrollSnapList.Horizontal>
        <Container.ItemFeed
          ?nextToken
          ?itemId
          isActive={Tab.equal(tab, Feed)}
          onVisibleItemChange=handleVisibleItemChange
        />
      </ScrollSnapList.Item>,
    |]
  </ScrollSnapList.Container>;
};
