open Lib.Styles;
open Externals;

[@react.component]
let make = (~itemId, ~nextToken=?) => {
  <Container.Items
    renderItem={(~itemId, ~isActive) =>
      <Container.Item
        itemId
        isActive
        style={
          style()
          ->unsafeStyle([
              ("scrollSnapStop", "always"),
              ("scrollSnapAlign", "start"),
            ])
        }
      />
    }
    renderContainer={(~onScroll, ~children) =>
      <div
        onScroll
        className={cn(["w-screen", "h-screen", "overflow-y-scroll"])}
        style={
          style()
          ->unsafeStyle([
              ("scrollSnapType", "y mandatory"),
              ("scrollSnapStop", "always"),
            ])
        }>
        children
      </div>
    }
    renderLoading={() => <Progress />}
    renderError={() => <Error />}
    onChange={(~nextToken, ~itemId, ()) =>
      Js.log3("onChange", nextToken, itemId)
    }
    ?nextToken
    itemId
  />;
};
