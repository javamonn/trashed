open Lib.Styles;

[@react.component]
let make = () => {
  <div
    className={cn([
      "w-full",
      "h-full",
      "bg-brandBlue",
      "flex",
      "justify-center",
      "items-center",
      "flex-col",
      "relative",
    ])}>
    <Marquee
      className={cn([
        "text-white",
        "font-bold",
        "italic",
        "uppercase",
        "text-6xl",
        "leading-none",
        "text-center",
        "absolute",
        "top-0",
        "left-0",
        "right-0",
      ])}
      scrollAmount=20>
      {React.string("Coming Soon to NYC")}
    </Marquee>
    <div className={cn(["my-32", "h-56", "w-56"])}>
      <SVG
        className={cn(["w-full", "h-full"])}
        icon=SVG.trashIconOnBackground
        placeholderViewBox="0 0 224 224"
      />
    </div>
    <Marquee
      className={cn([
        "text-white",
        "font-bold",
        "italic",
        "uppercase",
        "text-6xl",
        "leading-none",
        "text-center",
        "absolute",
        "bottom-0",
        "left-0",
        "right-0",
      ])}
      scrollAmount=20
      direction="right">
      {React.string("Coming Soon to NYC")}
    </Marquee>
  </div>;
};
