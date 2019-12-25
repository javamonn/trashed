open Lib.Styles;
open Externals;

module NearbyItemsQuery = [%graphql
  {|
    query NearbyItems($location: LocationInput, $m: Int, $nextToken: String)  {
      nearbyItems(location: $location, m: $m, nextToken: $nextToken, limit: 30) {
        nextToken
        items {
          id
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
  let (ignoreGeolocation, setIgnoreGeolocation) = React.useState(() => false);

  let location =
    switch (geolocationPermission) {
    | Service.Permission.PermissionGranted(Some(pos)) when !ignoreGeolocation =>
      Some({
        "lat": pos->Geolocation.coordsGet->Geolocation.latitudeGet,
        "lon": pos->Geolocation.coordsGet->Geolocation.longitudeGet,
      })
    | _ => None
    };

  let (query, _) =
    ApolloHooks.useQuery(
      ~fetchPolicy=ApolloHooksTypes.CacheAndNetwork,
      ~variables=NearbyItemsQuery.makeVariables(~location?, ~m=1000, ()),
      NearbyItemsQuery.definition,
    );

  let _ =
    React.useEffect1(
      () => {
        let _ =
          switch (query) {
          | Data(data) =>
            let items =
              data##nearbyItems
              ->Belt.Option.flatMap(l => l##items)
              ->Belt.Option.map(i => i->Belt.Array.keepMap(i => i));
            let newNextToken =
              data##nearbyItems->Belt.Option.flatMap(l => l##nextToken);
            let newItemId =
              switch (itemId) {
              | Some(itemId) => Some(itemId)
              | None =>
                items
                ->Belt.Option.flatMap(items => items->Belt.Array.get(0))
                ->Belt.Option.map(item => item##id)
              };

            /** No items, refetch without coordinates */
            let _ =
              setIgnoreGeolocation(ignoreGeolocation =>
                switch (items) {
                | None => true
                | Some(items) when Js.Array.length(items) === 0 => true
                | Some(_) => ignoreGeolocation
                }
              );

            if (newItemId !== itemId || newNextToken !== nextToken) {
              onVisibleItemChange(
                ~nextToken=newNextToken,
                ~itemId=newItemId,
                (),
              );
            };
          | Loading
          | Error(_)
          | NoData => ()
          };
        None;
      },
      [|query|],
    );

  let handleIdxChange = (~itemWindow, ~itemId, idx) => {
    switch (Belt.Array.get(itemWindow, idx)) {
    | Some(Some(item)) when item##id !== itemId =>
      let _ = onVisibleItemChange(~nextToken, ~itemId=Some(item##id), ());
      ();
    | _ => ()
    };
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

    switch (activeItemIdx, activeItemId) {
    | (Some(activeItemIdx), Some(activeItemId)) =>
      let itemWindow =
        Belt.Array.concatMany([|
          Belt.Array.makeBy(activeItemIdx - 1, _ => None),
          items
          ->Belt.Array.slice(~offset=max(0, activeItemIdx - 1), ~len=3)
          ->Belt.Array.map(item => Some(item)),
        |]);
      let itemWindowIdx =
        itemWindow
        ->Belt.Array.getIndexBy(
            fun
            | Some(item) => item##id === activeItemId
            | None => false,
          )
        ->Belt.Option.getExn;

      let isItemLastInFeed =
        itemWindowIdx === Js.Array.length(itemWindow) - 1;
      let isGeolocationRequired =
        switch (geolocationPermission) {
        | Service.Permission.Unknown
        | Service.Permission.PermissionGranted(Some(_)) => false
        | _ => true
        };

      <>
        <DelayedMount timeout=3000>
          <Notification.GeolocationRequired
            _in={!isItemLastInFeed && isGeolocationRequired}
            onClick={_ => {
              let _ = onPromptGeolocation();
              ();
            }}
            permission=geolocationPermission
          />
        </DelayedMount>
        <DelayedMount timeout=3000 paused={!isItemLastInFeed}>
          <Notification.LastItemInFeed
            _in=isItemLastInFeed
            onClick={_ => {
              let _ =
                Webapi.Dom.location
                |> Webapi.Dom.Location.assign("/item/feed");
              ();
            }}
          />
        </DelayedMount>
        <ScrollSnapList.Container
          direction=ScrollSnapList.Vertical
          onIdxChange={handleIdxChange(~itemWindow, ~itemId=activeItemId)}
          initialIdx=itemWindowIdx>
          {itemWindow->Belt.Array.map(
             fun
             | None =>
               <ScrollSnapList.Item direction=ScrollSnapList.Vertical />
             | Some(item) =>
               <ScrollSnapList.Item
                 key={item##id} direction=ScrollSnapList.Vertical>
                 <Container_Item
                   key={item##id}
                   itemFragment=item##itemFragment
                   autoPlay={item##id === activeItemId && isActive}
                 />
               </ScrollSnapList.Item>,
           )}
        </ScrollSnapList.Container>
      </>;
    | _ when Array.length(items) === 0 =>
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
    | _ => <Error />
    };
  };
};
