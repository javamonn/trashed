open Lib.Styles;

module RawErrorBoundary = {
  [@bs.module "./raw-error-boundary.js"] [@react.component]
  external make:
    (
      ~onComponentDidCatch: Js.Exn.t => unit,
      ~renderErrorBoundary: unit => React.element,
      ~children: React.element
    ) =>
    React.element =
    "default";
};

[@react.component]
let make = (~children) => {
  let handleTryAgain = () => {
    let _ = ReasonReactRouter.replace("/item/feed");
    let _ = Webapi.Dom.(window |> Window.location |> Location.reload);
    ();
  };
  <RawErrorBoundary
    renderErrorBoundary={() =>
      <div
        className={cn([
          "w-screen",
          "h-screen",
          "bg-brandBlue",
          "flex",
          "justify-center",
          "items-center",
          "flex-col",
          "relative",
        ])}>
        <Marquee
          className={cn([
            "text-black",
            "font-bold",
            "italic",
            "uppercase",
            "text-5xl",
            "leading-none",
            "text-center",
            "absolute",
            "top-0",
            "left-0",
            "right-0",
          ])}
          scrollAmount=20>
          {React.string("Error!")}
        </Marquee>
        <SVG
          className={cn(["h-56", "w-56", "mb-12"])}
          placeholderViewBox="0 0 128 128"
          icon=SVG.emojiWarning
        />
        <Button
          onClick={_ => handleTryAgain()}
          className={cn([
            "w-auto",
            "h-auto",
            "z-10",
            "flex",
            "justify-center",
            "items-center",
          ])}>
          <span
            className={cn([
              "py-4",
              "px-12",
              "font-bold",
              "italic",
              "uppercase",
              "underline",
              "text-4xl",
              "leading-none",
              "text-black",
            ])}>
            {React.string("Try Again")}
          </span>
        </Button>
        <Marquee
          className={cn([
            "text-black",
            "font-bold",
            "italic",
            "uppercase",
            "text-5xl",
            "leading-none",
            "text-center",
            "absolute",
            "bottom-0",
            "left-0",
            "right-0",
          ])}
          scrollAmount=20
          direction="right">
          {React.string("Error!")}
        </Marquee>
      </div>
    }
    onComponentDidCatch={error => {
      let _ = Sentry.captureException(error);
      ();
    }}>
    children
  </RawErrorBoundary>;
};
