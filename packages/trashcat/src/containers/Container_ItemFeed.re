open Lib.Utils;
open Lib.Styles;
open Externals;

module NearbyItemsQuery = [%graphql
  {|
    query NearbyItems($location: LocationInput, $m: Int, $nextToken: String)  {
      nearbyItems(location: $location, m: $m, nextToken: $nextToken, limit: 30) {
        nextToken
        items {
          id
          location {
            lat
            lon
          }
          createdAt
          ...Container_Item.GetItemFragment.ItemFragment @bsField(name: "itemFragment")
        }
      }
    }
  |}
];

[@react.component]
let make = (~isActive, ~onVisibleItemChange, ~itemId=?, ~nextToken=?) => {
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

  let (query, _fullQuery) =
    ApolloHooks.useQuery(
      ~fetchPolicy=ApolloHooksTypes.CacheAndNetwork,
      ~variables=NearbyItemsQuery.makeVariables(~location?, ~m=1000, ()),
      NearbyItemsQuery.definition,
    );

  let _ =
    React.useEffect1(
      () => {
        let (items, newNextToken) =
          switch (query) {
          | Data(data) =>
            let items =
              data##nearbyItems
              ->Belt.Option.flatMap(l => l##items)
              ->Belt.Option.map(i => i->Belt.Array.keepMap(i => i));
            let nextToken =
              data##nearbyItems->Belt.Option.flatMap(l => l##nextToken);
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
              onVisibleItemChange(
                ~nextToken=newNextToken,
                ~itemId=newItemId,
                (),
              );
            };
          | _ => ()
          };
        None;
      },
      [|query|],
    );

  let handleIdxChange = (~itemWindow, ~itemId, idx) =>
    switch (Belt.Array.get(itemWindow, idx)) {
    | Some(Some(item)) when item##id !== itemId =>
      let _ = onVisibleItemChange(~nextToken, ~itemId=Some(item##id), ());
      ();
    | _ => ()
    };

  switch (query) {
  | Loading =>
    <div
      className={cn([
        "w-screen",
        "h-screen",
        "flex",
        "justify-center",
        "items-center",
      ])}>
      <Progress />
    </div>
  | Error(_)
  | NoData => <Error />
  | Data(data) =>
    let items =
      data##nearbyItems
      ->Belt.Option.flatMap(l => l##items)
      ->Belt.Option.map(i => i->Belt.Array.keepMap(i => i))
      ->Belt.Option.getWithDefault([||]);

    let activeItemId =
      switch (itemId, Belt.Array.get(items, 0)) {
      | (Some(itemId), _) => Some(itemId)
      | (None, Some(item)) => Some(item##id)
      | (None, None) => None
      };
    let activeItemIdx =
      activeItemId->Belt.Option.flatMap(activeItemId =>
        items->Belt.Array.getIndexBy(i => activeItemId === i##id)
      );
    let activeItem =
      activeItemIdx->Belt.Option.flatMap(Belt.Array.get(items));
    let itemWindow =
      activeItemIdx->Belt.Option.map(activeItemIdx =>
        Belt.Array.makeBy(activeItemIdx + 2, idx =>
          if (idx <= activeItemIdx + 1 && idx >= activeItemIdx - 1) {
            items->Belt.Array.get(idx);
          } else {
            None;
          }
        )
      );
    switch (itemWindow, activeItem, activeItemIdx, activeItemId) {
    | (
        Some(itemWindow),
        Some(activeItem),
        Some(activeItemIdx),
        Some(activeItemId),
      ) =>
      <>
        <ItemTopOverlay onPromptGeolocation geolocationPermission />
        <ScrollSnapList.Container
          direction=ScrollSnapList.Vertical
          onIdxChange={handleIdxChange(~itemWindow, ~itemId=activeItemId)}
          initialIdx=activeItemIdx>
          {itemWindow->Belt.Array.map(item =>
             switch (item) {
             | Some(item) =>
               <ScrollSnapList.Item
                 key=item##id direction=ScrollSnapList.Vertical>
                 <Container_Item
                   key=item##id
                   itemFragment=item##itemFragment
                   autoPlay={item##id === activeItemId && isActive}
                 />
               </ScrollSnapList.Item>
             | None =>
               <ScrollSnapList.Item direction=ScrollSnapList.Vertical />
             }
           )}
        </ScrollSnapList.Container>
        <ItemBottomOverlay item=activeItem />
      </>
    | (_, _, _, _) => <Error />
    };
  };
};
