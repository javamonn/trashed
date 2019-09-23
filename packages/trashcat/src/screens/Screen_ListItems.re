open Lib.Styles;

module ListItemsQueryConfig = [%graphql
  {|
    query ListItems {
      listItems {
        items {
          id
          video {
            id
            thumbnail {
              bucket 
              key
              region
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
let make = () => {
  let (query, _fullQuery) = ListItemsQuery.use();

  switch (query) {
  | Loading =>
    <div
      className={cn([
        "w-screen",
        "h-screen",
        "flex",
        "justify-center",
        "items-center",
        "flex-col",
      ])}>
      <Progress />
    </div>
  | Data(data) =>
    data##listItems
    ->Belt.Option.flatMap(listItems => listItems##items)
    ->Belt.Option.map(items => {
        let rows =
          items
          ->Belt.Array.keep(Belt.Option.isSome)
          ->Belt.Array.map(Belt.Option.getExn)
          ->Belt.Array.reduce([||], (acc, item) =>
              switch (Belt.Array.get(acc, Belt.Array.length(acc) - 1)) {
              | Some(last) when Belt.Array.length(last) < 2 =>
                let _ = last |> Js.Array.push(item);
                acc;
              | Some(_)
              | None =>
                let _ = acc |> Js.Array.push([|item|]);
                acc;
              }
            )
          ->Belt.Array.map(row =>
              <div className={cn(["flex", "flex-row", "items-stretch"])}>
                {row
                 ->Belt.Array.mapWithIndex((idx, item) =>
                     <Link
                       className={cn([
                         "flex-1",
                         "h-48",
                         "bg-brandPrimary",
                         "mr-4"->Cn.ifTrue(idx < Belt.Array.length(row) - 1),
                       ])}
                       href={"/item/" ++ item##id}>
                       {React.string(item##id)}
                     </Link>
                   )
                 ->React.array}
              </div>
            )
          ->React.array;

        <div className={cn(["w-screen", "h-screen"])}> rows </div>;
      })
    ->Belt.Option.getWithDefault(React.string("Nothing here!"))
  | NoData
  | Error(_) => React.string("Nothing here!")
  };
};
