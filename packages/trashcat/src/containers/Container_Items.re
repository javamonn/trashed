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
  | (Some(itemId), _) => renderItem(~itemId)
  | (None, Loading) => renderLoading()
  | (None, Error(_))
  | (None, Loading) => renderError()
  };
};
