open Lib.Styles;

module GetItemQueryConfig = [%graphql
  {|
    query GetItem($itemId: ID!) {
      getItem(id: $itemId) {
        id
        video {
          id
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
            s3Object={
              item##video##files
              ->Belt.Array.map(i => i##file->S3ObjectResolver.S3Object.fromJs)
            }
            render={(~result) =>
              <VideoSurface
                src={
                  Belt.Array.zip(result, item##video##files)
                  ->Belt.Array.map(((src, file)) => (src, file##mimeType))
                  ->VideoSurface.srcElement
                }
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
