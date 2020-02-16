open Lib.Styles;

let renderLink = (~text, ~href, ~isLocal) => {
  let children =
    <>
      <div
        className={cn(["border-white", "border-4", "w-8", "h-8", "mr-4"])}
      />
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
    </>;

  isLocal
    ? <Link href className={cn(["flex", "flex-row", "my-8", "items-center"])}>
        children
      </Link>
    : <a href className={cn(["flex", "flex-row", "my-8", "items-center"])}>
        children
      </a>;
};

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
        <span className={cn(["text-lg", "font-bold"])}>
          {React.string(
             "trashed helps you find and reclaim objects that have been thrown out by someone else.",
           )}
        </span>
        <div className={cn(["my-4"])} />
        <span className={cn(["text-lg", "font-bold"])}>
          {React.string(
             "trashed is a free, decentralized, and efficient peer-to-peer recycling system.",
           )}
        </span>
        <div className={cn(["my-4"])} />
        <span className={cn(["text-lg", "font-bold"])}>
          {React.string("trashed is dumpster diving for the digital age.")}
        </span>
      </div>
      <div
        className={cn([
          "bg-black",
          "flex-col",
          "flex",
          "justify-center",
          "items-center",
          "py-3",
        ])}>
        <div>
          {renderLink(~text="App", ~href="/item/feed", ~isLocal=true)}
          {renderLink(~text="About", ~href="/about", ~isLocal=true)}
          {renderLink(
             ~text="Blog",
             ~href="https://blog.trashed.today",
             ~isLocal=false,
           )}
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
      placeholderClassName={cn([
        "flex-shrink-0",
        "min-h-full",
        "h-full",
        "block",
        "bg-brandBlue",
        "py-2",
        "px-4",
      ])}
    />
  </div>;
};
