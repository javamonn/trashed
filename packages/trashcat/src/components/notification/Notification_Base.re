open Lib.Styles;

[@react.component]
let make = (~onClick=?, ~style=?, ~renderIcon, ~renderText) =>
  <div
    ?style
    className={cn([
      "absolute",
      "inset-x-0",
      "top-0",
      "h-24",
      "my-4",
      "mx-6",
      "z-20",
      "flex",
      "flex-row",
    ])}>
    <Button
      className={cn(["flex-auto", "flex", "items-center", "justify-start"])}
      ?onClick>
      <div
        className={cn([
          "h-24",
          "w-24",
          "flex-shrink-0",
          "flex",
          "items-center",
          "justify-center",
          "p-6",
        ])}>
        {renderIcon()}
      </div>
      <div
        className={cn([
          "text-black",
          "font-bold",
          "italic",
          "leading-none",
          "text-xl",
          "mr-6",
        ])}>
        {renderText()}
      </div>
    </Button>
  </div>;
