open Lib.Styles;

[@react.component]
let make = (~className=?, ~renderIcon, ~onClick) => {
  <div
    className={cn([
      "flex",
      "justify-center",
      "items-center",
      className->Cn.unpack,
    ])}
    onClick>
    {renderIcon()}
    <div
      style={style(
        ~bottom=rem(0.),
        ~right=rem(0.),
        ~top=rem(0.375),
        ~left=rem(0.375),
        (),
      )}
      className={cn(["absolute", "bg-brandYellow"])}
    />
    <div
      style={style(
        ~top=rem(0.),
        ~left=rem(0.),
        ~right=rem(0.375),
        ~bottom=rem(0.375),
        (),
      )}
      className={cn(["absolute", "bg-black"])}
    />
  </div>;
};
