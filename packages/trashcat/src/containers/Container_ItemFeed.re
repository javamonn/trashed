open Lib.Styles;
open Externals;

[@bs.deriving jsConverter]
type error = [
  | [@bs.as "InvalidState_ExpectedData"] `InvalidState_ExpectedData
  | [@bs.as "InvalidState_ExpectedActiveItem"] `InvalidState_ExpectedActiveItem
];

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

module QueryJson = {
  type dict = Js.Dict.t(string);

  [@decco]
  type item = {
    id: string,
    itemFragment: [@decco.codec Decco.Codecs.magic] dict,
  };

  [@decco]
  type nearbyItems = {
    nextToken: option(string),
    items: option(array(item)),
  };

  [@decco]
  type t = {nearbyItems: option(nearbyItems)};

  let decode = t_decode;
  let encode = t_encode;
};

module Location = {
  let make = pos => {
    {
      "lat": pos->Geolocation.coordsGet->Geolocation.latitudeGet,
      "lon": pos->Geolocation.coordsGet->Geolocation.longitudeGet,
    };
  };

  let parse = json => {
    let decodeField = key =>
      json
      ->Js.Json.decodeObject
      ->Belt.Option.flatMap(o => o->Js.Dict.get(key))
      ->Belt.Option.flatMap(Js.Json.decodeNumber);
    switch (decodeField("lat"), decodeField("lon")) {
    | (Some(lat), Some(lon)) => Some({"lat": lat, "lon": lon})
    | _ => None
    };
  };

  let eq = (l1, l2) => l1##lat === l2##lat && l1##lon === l2##lon;
};

[@react.component]
let make = (~isActive, ~onVisibleItemChange, ~itemId=?, ~nextToken=?) => {
  let (geolocationPermission, onPromptGeolocation, _) =
    Service.Permission.Geolocation.use();

  let (queryVariables, setQueryVariables) =
    React.useState(() => {
      let location =
        switch (geolocationPermission) {
        | Service.Permission.PermissionGranted(Some(pos)) =>
          Some(Location.make(pos))
        | _ => None
        };
      NearbyItemsQuery.makeVariables(~location?, ~m=1000, ());
    });
  let fetchMoreVariablesRef = React.useRef(queryVariables);

  let (query, fullQuery) =
    ApolloHooks.useQuery(
      ~fetchPolicy=ApolloHooksTypes.CacheAndNetwork,
      ~variables=queryVariables,
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
              setQueryVariables(variables =>
                switch (items) {
                | None => NearbyItemsQuery.makeVariables(~m=1000, ())
                | Some(items) when Js.Array.length(items) === 0 =>
                  NearbyItemsQuery.makeVariables(~m=1000, ())
                | Some(_) => variables
                }
              );

            /**
             * If itemId is unset, or if nextToken has been updated,
             * propagate changes into URL.
             */
            let _ =
              if (!Belt.Option.eq(newItemId, itemId, (==))
                  || !Belt.Option.eq(newNextToken, nextToken, (==))) {
                onVisibleItemChange(
                  ~nextToken=newNextToken,
                  ~itemId=newItemId,
                  (),
                );
              };
            ();
          | Loading
          | Error(_)
          | NoData => ()
          };
        None;
      },
      [|query|],
    );

  let updateQuery = (previousResult, nextFetchMore) => {
    let prev = previousResult->QueryJson.decode;
    let next =
      switch (nextFetchMore->ApolloHooksQuery.fetchMoreResultGet) {
      | Some(nextResult) => nextResult->QueryJson.decode
      | None => Decco.error("fetchMoreResult is None", Js.Json.null)
      };

    let result = switch (prev, next) {
      | (Belt.Result.Ok(prev), Belt.Result.Ok(next)) => () /** combine prev and next **/
      | (Belt.Result.Error(_), Belt.Result.Error(_))
      | (Belt.Result.Ok(_), Belt.Result.Error(_)) => () /** Only prev **/
      | (Belt.Result.Error(_), Belt.Result.Ok(next)) => () /** Only next **/
    }
  };

  /**
    let parseItems = json =>
      json
      ->Js.Json.decodeObject
      ->Belt.Option.flatMap(o => o->Js.Dict.get("nearbyItems"))
      ->Belt.Option.flatMap(Js.Json.decodeObject)
      ->Belt.Option.flatMap(o => o->Js.Dict.get("items"))
      ->Belt.Option.flatMap(Js.Json.decodeArray)
      ->Belt.Option.map(arr => arr->Belt.Array.keepMap(Js.Json.decodeObject))
      ->Belt.Option.getWithDefault([||]);
    let previousItems = previousResult->parseItems;
    let items =
      fetchMoreOptions
      ->ApolloHooksQuery.fetchMoreResultGet
      ->Belt.Option.map(parseItems)
      ->Belt.Option.getWithDefault([||]);
    let nextToken =
      fetchMoreOptions
      ->ApolloHooksQuery.fetchMoreResultGet
      ->Belt.Option.flatMap(Js.Json.decodeObject)
      ->Belt.Option.flatMap(o => o->Js.Dict.get("nearbyItems"));

    let activeItemIdx =
      previousItems->Belt.Array.getIndexBy(item => {
        switch (
          item->Js.Dict.get("id")->Belt.Option.flatMap(Js.Json.decodeString),
          itemId,
        ) {
        | (Some(parsedItemId), Some(itemId)) when parsedItemId === itemId =>
          true
        | _ => false
        }
      });
    let updatedItems =
      switch (activeItemIdx) {
      | Some(idx) =>
        Belt.Array.(
          concat(slice(previousItems, ~offset=0, ~len=idx + 1), items)
        )
      | None => Belt.Array.concat(previousItems, items)
      };
      **/

  let _ =
    React.useEffect1(
      () => {
        let location =
          fetchMoreVariablesRef
          ->React.Ref.current
          ->Js.Json.decodeObject
          ->Belt.Option.flatMap(o => o->Js.Dict.get("location"))
          ->Belt.Option.flatMap(Location.parse);

        let nextLocation =
          switch (geolocationPermission) {
          | Service.Permission.PermissionGranted(Some(pos)) =>
            Some(Location.make(pos))
          | _ => None
          };

        if (!Belt.Option.eq(location, nextLocation, Location.eq)) {
          let nextVariables =
            NearbyItemsQuery.makeVariables(
              ~location=?nextLocation,
              ~m=1000,
              (),
            );
          let _ = fetchMoreVariablesRef->React.Ref.setCurrent(nextVariables);
          let _ =
            fullQuery.fetchMore(~variables=nextVariables, ~updateQuery, ());
          ();
        };
        None;
      },
      [|geolocationPermission|],
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
        "w-full",
        "h-full",
        "flex",
        "justify-center",
        "items-center",
      ])}>
      <Progress />
    </div>
  | Error(_)
  | NoData => `InvalidState_ExpectedData->errorToJs->Js.Exn.raiseError
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
            onClick={_ => {Js.log("LastItemInFeed Clicked!")}}
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
          "w-full",
          "h-full",
          "flex",
          "justify-center",
          "items-center",
          "flex-col",
        ])}>
        <Progress />
      </div>
    | _ => `InvalidState_ExpectedActiveItem->errorToJs->Js.Exn.raiseError
    };
  };
};
