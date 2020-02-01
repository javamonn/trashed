open Lib.Styles;

let renderLink = (~text, ~href) =>
  <Link href className={cn(["flex", "flex-row", "my-8", "items-center"])}>
    <div className={cn(["border-white", "border-4", "w-8", "h-8", "mr-4"])} />
    <div
      className={cn([
        "text-white",
        "font-bold",
        "italic",
        "underline",
        "uppercase",
        "text-4xl",
        "leading-none",
      ])}>
      {React.string(text)}
    </div>
  </Link>;

[@react.component]
let make = () => {
  <div className={cn(["w-full", "h-full", "flex", "flex-row"])}>
    <div className={cn(["flex", "flex-auto", "flex-col"])}>
      <div
        className={cn([
          "bg-brandYellow",
          "flex-1",
          "flex",
          "flex-col",
          "justify-center",
          "items-center",
          "py-4",
          "px-6",
        ])}>
        <span className={cn(["text-xl", "font-bold", "mb-8"])}>
          {React.string(
             "trashed helps you find and reclaim objects that have been thrown out.",
           )}
        </span>
        <span className={cn(["text-xl", "font-bold"])}>
          {React.string("trashed is dumpster diving for the digital age.")}
        </span>
      </div>
      <div
        className={cn([
          "bg-black",
          "flex-col",
          "flex-1",
          "flex",
          "justify-center",
          "items-center",
        ])}>
        <div>
          {renderLink(~text="App", ~href="/coming-soon")}
          {renderLink(~text="About", ~href="/coming-soon")}
          {renderLink(~text="Blog", ~href="https://blog.trashed.today")}
        </div>
      </div>
    </div>
    <SVG
      className={cn([
        "min-h-full",
        "h-full",
        "block",
        "bg-brandBlue",
        "py-2",
        "px-4",
      ])}
      icon=SVG.trashedText
      placeholderViewBox="0 0 109 689"
      placeholderClassName={cn(["bg-brandBlue"])}
    />
  </div>;
};
