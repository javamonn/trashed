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
      ~onChange,
      ~itemId=?,
      ~nextToken=?,
    ) => {
  let (query, fullQuery) =
    ListItemsQuery.use(
      ~variables=
        ListItemsQueryConfig.make(~limit=30, ~nextToken?, ())##variables,
      (),
    );

  let _ =
    React.useEffect1(
      () => {
        let (items, nextToken) =
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
          | Data(data) =>
            switch (itemId) {
            | Some(itemId) => onChange(~nextToken, ~itemId=Some(itemId), ())
            | None =>
              onChange(
                ~nextToken,
                ~itemId=
                  items
                  ->Belt.Option.flatMap(items => items->Belt.Array.get(0))
                  ->Belt.Option.map(item => item##id),
                (),
              )
            }
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
        [|
          items->Belt.Array.get(itemIdx - 1),
          items->Belt.Array.get(itemIdx),
          items->Belt.Array.get(itemIdx + 1),
        |]
        ->Belt.Array.keepMap(i => i);
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
              idx |> Belt.Array.get(itemWindow)
            )
          ) {
          | Some(item) =>
            let _ = onChange(~nextToken, ~itemId=Some(item##id), ());
            ();
          | None => ()
          };
        ();
      };
      renderContainer(
        ~onScroll=handleScroll,
        ~children=
          itemWindow
          ->Belt.Array.map(i =>
              renderItem(~itemId=i##id, ~isActive=i##id === itemId)
            )
          ->React.array,
      );
    | None => renderItem(~itemId)
    }
  | (Some(itemId), _) => renderItem(~itemId)
  | (None, Loading) => renderLoading()
  | (None, Error(_))
  | (None, Loading) => renderError()
  };
};
