open Lib.Styles;
open Externals;

module GraphQL = Container_ItemFeed_GraphQL;

[@bs.deriving jsConverter]
type error = [
  | [@bs.as "InvalidState_ExpectedData"] `InvalidState_ExpectedData
  | [@bs.as "InvalidState_ExpectedActiveItem"] `InvalidState_ExpectedActiveItem
  | [@bs.as "InvalidState_UnableToParseQuery"] `InvalidState_UnableToParseQuery
];

[@react.component]
let make = (~isActive, ~onVisibleItemChange, ~itemId=?, ~nextToken=?) => {
  let (geolocationPermission, onPromptGeolocation, _) =
    Service.Permission.Geolocation.use();

  let (queryVariables, setQueryVariables) =
    React.useState(() =>
      GraphQL.Variables.{
        location:
          switch (geolocationPermission) {
          | Service.Permission.PermissionGranted(Some(pos)) =>
            GraphQL.Variables.{
              lat: Geolocation.(pos->coordsGet->latitudeGet),
              lon: Geolocation.(pos->coordsGet->longitudeGet),
            }
            ->Js.Option.some
          | _ => None
          },
        m: Some(1000),
        nextToken: None,
      }
    );

  let fetchMoreVariablesRef = queryVariables->React.useRef;

  let (query, fullQuery) =
    ApolloHooks.useQuery(
      ~fetchPolicy=ApolloHooksTypes.CacheAndNetwork,
      ~variables=GraphQL.Variables.encode(queryVariables),
      GraphQL.Config.definition,
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
              setQueryVariables(variables => {
                let newVariables =
                  switch (items) {
                  | None => GraphQL.Variables.make(~m=1000, ())
                  | Some(items) when Js.Array.length(items) === 0 =>
                    GraphQL.Variables.make(~m=1000, ())
                  | Some(_) => variables
                  };
                let _ =
                  fetchMoreVariablesRef->React.Ref.setCurrent(newVariables);
                newVariables;
              });

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

  let updateQuery =
      (
        ~truncatePreviousResultToActiveItem=false,
        previousResult,
        nextFetchMore,
      ) => {
    let prev = previousResult->GraphQL.Json.decode;
    let next =
      switch (nextFetchMore->ApolloHooksQuery.fetchMoreResultGet) {
      | Some(nextResult) => nextResult->GraphQL.Json.decode
      | None => Decco.error("fetchMoreResult is None", Js.Json.null)
      };

    let nearbyItems =
      switch (prev, next) {
      | (Belt.Result.Ok(prev), Belt.Result.Ok(next)) =>
        let prevNearbyItems =
          prev.nearbyItems
          ->Belt.Option.flatMap(nearbyItems => nearbyItems.items)
          ->Belt.Option.getWithDefault([||]);
        let retainedPrevItems =
          switch (
            Belt.Array.getIndexBy(prevNearbyItems, item =>
              Belt.Option.eq(Js.Dict.get(item, "id"), itemId, (===))
            )
          ) {
          | Some(idx) when truncatePreviousResultToActiveItem =>
            Belt.Array.slice(prevNearbyItems, ~offset=0, ~len=idx + 1)
          | _ => prevNearbyItems
          };

        Belt.Result.Ok(
          GraphQL.Json.{
            typename:
              next.nearbyItems
              ->Belt.Option.flatMap(nearbyItems => nearbyItems.typename),
            nextToken:
              next.nearbyItems
              ->Belt.Option.flatMap(nearbyItems => nearbyItems.nextToken),
            items:
              Some(
                Belt.Array.concat(
                  retainedPrevItems,
                  next.nearbyItems
                  ->Belt.Option.flatMap(nearbyItems => nearbyItems.items)
                  ->Belt.Option.getWithDefault([||]),
                ),
              ),
          },
        );
      | (Belt.Result.Error(_), Belt.Result.Error(_))
      | (Belt.Result.Ok(_), Belt.Result.Error(_))
      | (Belt.Result.Error(_), Belt.Result.Ok(_)) =>
        Belt.Result.Error(`InvalidState_UnableToParseQuery)
      };
    switch (nearbyItems) {
    | Belt.Result.Ok(nearbyItems) =>
      {nearbyItems: Some(nearbyItems)}->GraphQL.Json.encode
    | Belt.Result.Error(err) =>
      let _ =
        try(err->errorToJs->Js.Exn.raiseError) {
        | Js.Exn.Error(err) => Sentry.captureException(err)
        };
      previousResult;
    };
  };

  let _ =
    React.useEffect1(
      () => {
        let variables = React.Ref.current(fetchMoreVariablesRef);
        let nextLocation =
          switch (geolocationPermission) {
          | Service.Permission.PermissionGranted(Some(pos)) =>
            Some(
              GraphQL.Variables.{
                lat: Geolocation.(pos->coordsGet->latitudeGet),
                lon: Geolocation.(pos->coordsGet->longitudeGet),
              },
            )
          | _ => None
          };

        if (!
              Belt.Option.eq(variables.location, nextLocation, (l1, l2) =>
                l1.lat === l2.lat && l1.lon === l2.lon
              )) {
          let _ =
            fetchMoreVariablesRef->React.Ref.setCurrent(
              GraphQL.Variables.{
                nextToken: None,
                m: Some(1000),
                location: nextLocation,
              },
            );
          let _ =
            fullQuery.fetchMore(
              ~variables=
                fetchMoreVariablesRef
                ->React.Ref.current
                ->GraphQL.Variables.encode,
              ~updateQuery=
                updateQuery(~truncatePreviousResultToActiveItem=true),
              (),
            );
          ();
        };
        None;
      },
      [|geolocationPermission|],
    );

  switch (fullQuery.data, fullQuery.loading, fullQuery.error) {
  | (None, true, None) =>
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
  | (None, false, None)
  | (_, _, Some(_)) =>
    `InvalidState_ExpectedData->errorToJs->Js.Exn.raiseError
  | (Some(data), _, _) =>
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

      let handleIdxChange = idx => {
        let _ =
          switch (Belt.Array.get(itemWindow, idx)) {
          | Some(Some(item)) when item##id !== activeItemId =>
            let _ =
              onVisibleItemChange(~nextToken, ~itemId=Some(item##id), ());
            ();
          | _ => ()
          };

        let shouldFetchMore =
          switch (query) {
          | Data(data) =>
            data##nearbyItems
            ->Belt.Option.flatMap(nearbyItems => nearbyItems##nextToken)
            ->Belt.Option.map(nextToken =>
                if (activeItemIdx
                    > int_of_float(
                        items->Js.Array.length->float_of_int *. 0.75,
                      )) {
                  let _ =
                    fetchMoreVariablesRef->React.Ref.setCurrent(
                      GraphQL.Variables.{
                        ...fetchMoreVariablesRef->React.Ref.current,
                        nextToken: Some(nextToken),
                      },
                    );
                  true;
                } else {
                  false;
                }
              )
            ->Belt.Option.getWithDefault(false)
          | _ => false
          };
        let _ =
          if (shouldFetchMore) {
            let _ =
              fullQuery.fetchMore(
                ~variables=
                  fetchMoreVariablesRef
                  ->React.Ref.current
                  ->GraphQL.Variables.encode,
                ~updateQuery=
                  updateQuery(~truncatePreviousResultToActiveItem=false),
                (),
              );
            ();
          };
        ();
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
                Webapi.Dom.(
                  window->Window.location->Location.setPathname("/item")
                );
              ();
            }}
          />
        </DelayedMount>
        <ScrollSnapList.Container
          direction=ScrollSnapList.Vertical
          onIdxChange=handleIdxChange
          initialIdx=itemWindowIdx>
          {itemWindow->Belt.Array.mapWithIndex((idx, item) =>
             switch (item) {
             | None =>
               <ScrollSnapList.Item
                 key={string_of_int(activeItemIdx + idx)}
                 direction=ScrollSnapList.Vertical
               />
             | Some(item) =>
               <ScrollSnapList.Item
                 key={item##id} direction=ScrollSnapList.Vertical>
                 <Container_Item
                   key={item##id}
                   itemFragment=item##itemFragment
                   autoPlay={item##id === activeItemId && isActive}
                 />
               </ScrollSnapList.Item>
             }
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
