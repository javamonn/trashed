open Lib.Styles;

module ItemPosted = {
  [@react.component]
  let make = (~style=?) =>
    <div
      ?style
      className={cn([
        "absolute",
        "inset-x-0",
        "top-0",
        "h-24",
        "my-4",
        "mx-6",
        "z-10",
        "flex",
        "flex-row",
      ])}>
      <Button
        onClick={_ev => ()}
        className={cn([
          "flex",
          "items-center",
          "justify-between",
          "flex-auto",
        ])}>
        <div
          className={cn([
            "h-24",
            "w-24",
            "flex-shrink-0",
            "flex-auto",
            "items-center",
            "justify-center",
            "px-6",
            "py-4"
          ])}>
          <SVG className={cn(["h-full", "w-full"])} icon=SVG.trashFavorite />
        </div>
        <div
          className={cn([
            "text-black",
            "font-bold",
            "italic",
            "leading-none",
            "text-xl",
            "mr-6",
            "flex-auto",
          ])}>
          {React.string("trash was posted!")}
        </div>
      </Button>
    </div>;
};

[@react.component]
let make = (~_in) => <Grow _in appear=true> <ItemPosted /> </Grow>;
