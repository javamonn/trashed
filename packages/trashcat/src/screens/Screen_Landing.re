open Lib.Styles;

let renderLink = (~text) =>
  <div className={cn(["flex", "flex-row", "my-2"])}>
    <div className={cn(["border-white", "border-4", "w-24", "h-24"])} />
    <div
      className={cn([
        "text-white",
        "font-bold",
        "italic",
        "underline",
        "uppercase",
        "text-4xl",
      ])}>
      {React.string(text)}
    </div>
  </div>;

[@react.component]
let make = () => {
  <div className={cn(["w-screen", "h-screen", "flex", "flex-row"])}>
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
        <span className={cn(["text-xl", "font-bold", "mb-6"])}>
          {React.string(
             "Trashed is a community that helps you find and replace trashed objects:",
           )}
        </span>
        <span className={cn(["text-xl", "font-bold"])}>
          {React.string("things that have been thrown out.")}
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
          {renderLink(~text="App")}
          {renderLink(~text="About")}
          {renderLink(~text="Blog")}
        </div>
      </div>
    </div>
    <div
      className={cn([
        "bg-brandBlue",
        "py-2",
        "px-6",
        "flex",
        "justify-center",
        "items-center",
        "flex-initial",
      ])}>
      <TrashedTextSVG className={cn(["w-full", "h-full"])} />
    </div>
  </div>;
};
