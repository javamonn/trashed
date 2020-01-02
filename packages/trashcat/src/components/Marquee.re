[@react.component]
let make = (~className=?, ~scrollAmount=?, ~direction=?, ~children) =>
  ReactDOMRe.createElementVariadic(
    "marquee",
    ~props=
      ReactDOMRe.objToDOMProps({
        "className": className,
        "scrollamount": scrollAmount,
        "direction": direction,
      }),
    [|children|],
  );
