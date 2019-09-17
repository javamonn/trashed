open Lib.Styles;

[@react.component]
let make = (~className=?, ~href, ~children) => {
  let onClick = ev => {
    ev->ReactEvent.Synthetic.preventDefault;
    let _ = ReasonReactRouter.push(href);
    ();
  };

  <a
    onClick
    className={cn(["no-underline", "cursor-pointer", Cn.unpack(className)])}
    href>
    children
  </a>;
};
