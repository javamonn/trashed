open Lib.Styles;

[@react.component]
let make = (~className=?, ~backgroundClassName=?, ~onClick=?, ~children) => {
  <div className={cn(["relative", className->Cn.unpack])} ?onClick>
    children
    <div
      style={style(
        ~bottom=rem(0.),
        ~right=rem(0.),
        ~top=rem(0.25),
        ~left=rem(0.25),
        ~zIndex="-1",
        (),
      )}
      className={cn(["absolute", "bg-black"])}
    />
    <div
      style={style(
        ~top=rem(0.),
        ~left=rem(0.),
        ~right=rem(0.25),
        ~bottom=rem(0.25),
        ~zIndex="-1",
        (),
      )}
      className={
        backgroundClassName->Belt.Option.getWithDefault(
          cn(["absolute", "bg-brandYellow"]),
        )
      }
    />
  </div>;
};
