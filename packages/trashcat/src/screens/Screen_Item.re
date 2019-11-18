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

  let handleReplaceUrlSearch = newSearch => {
    let _ =
      url.path
      |> Array.of_list
      |> Js.Array.joinWith("/")
      |> (path => "/" ++ path ++ "?" ++ newSearch)
      |> ReasonReactRouter.replace;
    ();
  };

  let handleScroll = ev => {
    let scrollLeft = ReactEvent.UI.target(ev)##scrollLeft;
    let windowWidth = Webapi.Dom.(window->Window.innerWidth);
    let tabs = Tab.ordered();
    let activeIdx =
      tabs
      ->Belt.Array.mapWithIndex((idx, _item) => idx * windowWidth)
      ->Belt.Array.getIndexBy(width => scrollLeft === width);

    let _ =
      switch (activeIdx) {
      | Some(activeIdx) =>
        let newTab = tabs[activeIdx];
        let pathArr = Array.of_list(url.path);
        if (newTab !== tab) {
          let _ =
            Belt.Array.(
              set(pathArr, length(pathArr) - 1, Tab.pathOf(newTab))
            );
          let _ =
            pathArr
            |> Js.Array.joinWith("/")
            |> (path => "/" ++ path ++ "?" ++ url.search)
            |> ReasonReactRouter.replace;
          ();
        };
      | None => ()
      };
    ();
  };

  <ScrollSnapList.Container
    direction=ScrollSnapList.Horizontal
    initialIdx={Tab.indexOf(tab)}
    onScroll=handleScroll>
    <ScrollSnapList.Item direction=ScrollSnapList.Horizontal>
      <Screen_NewItem isActive={Tab.equal(tab, New)} />
    </ScrollSnapList.Item>
    <ScrollSnapList.Item direction=ScrollSnapList.Horizontal>
      <Screen_ListItems
        ?nextToken
        ?itemId
        onReplaceUrlSearch=handleReplaceUrlSearch
        isActive={Tab.equal(tab, Feed)}
      />
    </ScrollSnapList.Item>
    <ScrollSnapList.Item direction=ScrollSnapList.Horizontal>
      <Screen_ComingSoon />
    </ScrollSnapList.Item>
  </ScrollSnapList.Container>;
};
