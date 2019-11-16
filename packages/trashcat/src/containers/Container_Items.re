open Lib.Utils;

module ListItemsQueryConfig = [%graphql
  {|
    query ListItems($limit: Int!, $nextToken: String)  {
      listItems(limit: $limit, nextToken: $nextToken) {
        nextToken
        items {
          id
          video {
            id
            thumbnail {
              bucket
              key
              region
            }
            files {
              file {
                bucket
                key
                region
              }
              mimeType
            }
          }
          location {
            lat
            lon
          }
        }
      }
    }
  |}
];

module ListItemsQuery = ReasonApolloHooks.Query.Make(ListItemsQueryConfig);

[@react.component]
let make =
    (
      ~renderItem,
      ~renderError,
      ~renderLoading,
      ~renderContainer,
      ~renderPlaceholder,
      ~onChange,
      ~itemId=?,
      ~nextToken=?,
    ) => {
  let (query, _fullQuery) =
    ListItemsQuery.use(
      ~variables=
        ListItemsQueryConfig.make(~limit=30, ~nextToken?, ())##variables,
      (),
    );

  let _ =
    React.useEffect1(
      () => {
        let (items, newNextToken) =
          switch (query) {
          | Data(data) =>
            let items =
              data##listItems
              ->Belt.Option.flatMap(l => l##items)
              ->Belt.Option.map(i => i->Belt.Array.keepMap(i => i));
            let nextToken =
              data##listItems->Belt.Option.flatMap(l => l##nextToken);
            (items, nextToken);
          | _ => (None, None)
          };
        let _ =
          switch (query) {
          | Data(_) =>
            let newItemId =
              switch (itemId) {
              | Some(itemId) => Some(itemId)
              | None =>
                items
                ->Belt.Option.flatMap(items => items->Belt.Array.get(0))
                ->Belt.Option.map(item => item##id)
              };
            if (newItemId !== itemId || newNextToken !== nextToken) {
              onChange(~nextToken=newNextToken, ~itemId=newItemId, ());
            };
          | _ => ()
          };
        None;
      },
      [|query|],
    );

  switch (itemId, query) {
  | (Some(itemId), Data(data)) =>
    switch (
      data##listItems
      ->Belt.Option.flatMap(l => l##items)
      ->Belt.Option.map(i => i->Belt.Array.keepMap(i => i))
    ) {
    | Some(items) =>
      let itemIdx =
        items
        ->Belt.Array.getIndexBy(i => itemId === i##id)
        ->Belt.Option.getExn;
      let itemWindow =
        Belt.Array.makeBy(itemIdx + 2, idx =>
          if (idx <= itemIdx + 1 && idx >= itemIdx - 1) {
            items->Belt.Array.get(idx);
          } else {
            None;
          }
        );
      let handleScroll = ev => {
        let scrollTop = ReactEvent.UI.target(ev)##scrollTop;
        let windowHeight = Webapi.Dom.(window->Window.innerHeight);
        let activeIdx =
          itemWindow
          ->Belt.Array.mapWithIndex((idx, _item) => idx * windowHeight)
          ->Belt.Array.getIndexBy(height => scrollTop === height);

        let _ =
          switch (
            activeIdx->Belt.Option.flatMap(idx =>
              Belt.Array.get(itemWindow, idx)->Belt.Option.flatMap(identity)
            )
          ) {
          | Some(item) when item##id !== itemId =>
            let _ = onChange(~nextToken, ~itemId=Some(item##id), ());
            ();
          | _ => ()
          };
        ();
      };
      renderContainer(
        ~onScroll=handleScroll,
        ~children=
          itemWindow
          ->Belt.Array.map(item =>
              switch (item) {
              | Some(item) =>
                renderItem(~itemId=item##id, ~isActive=item##id === itemId)
              | None => renderPlaceholder()
              }
            )
          ->React.array,
      );
    | None => renderItem(~itemId, ~isActive=true)
    }
  | (Some(itemId), _) => renderItem(~itemId, ~isActive=true)
  | (None, Loading)
  | (None, Data(_)) => renderLoading()
  | (None, Error(_)) => renderError()
  };
};
