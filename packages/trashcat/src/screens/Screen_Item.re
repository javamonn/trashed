open Lib.Styles;
open Externals;

[@react.component]
let make = (~itemId, ~nextToken=?) => {
  <div className={cn(["w-screen", "h-screen", "relative"])}>
    <Container.Items
      renderItem={(~itemId) => <Container.Item itemId />}
      renderLoading={() => <Progress />}
      renderError={() => <Error />}
      onChange={(~nextToken, ~itemId, ()) => ()}
      ?nextToken
      itemId
    />
  </div>;
};
