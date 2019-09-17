open Lib.Styles;

module GetItemQueryConfig = [%graphql
  {|
    query GetItem($itemId: ID!) {
      getItem(id: $itemId) {
        id
        file {
          bucket
          key
          region
        }
        location {
          lat
          lon
        }
      }
    }
  |}
];

module GetItemQuery = ReasonApolloHooks.Query.Make(GetItemQueryConfig);

[@react.component]
let make = (~itemId) => {
  let (query, _fullQuery) =
    GetItemQuery.use(
      ~variables=GetItemQueryConfig.make(~itemId, ())##variables,
      (),
    );

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
    data##getItem
    ->Belt.Option.map(item =>
        <div className={cn(["w-screen", "h-screen", "relative"])}>
          <S3ObjectResolver
            s3Object={S3ObjectResolver.S3Object.fromJs(item##file)}
            render={(~url) =>
              <VideoSurface
                src={VideoSurface.srcUrl(url)}
                autoPlay=true
                controls=true
              />
            }
          />
        </div>
      )
    ->Belt.Option.getWithDefault(React.string("Nothing here!"))
  | Error(_)
  | NoData => React.string("Nothing here!")
  };
};
