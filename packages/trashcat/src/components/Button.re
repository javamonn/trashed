[@react.component]
let make = (~className=?, ~onClick, ~children) =>
  <button
    type_="button"
    onClick
    className={Cn.make([
      "bg-white",
      "hover:bg-gray-100",
      "text-gray-800",
      "font-semibold",
      "py-2",
      "px-4",
      "border",
      "border-gray-400",
      "rounded",
      "shadow",
      className->Cn.unpack,
    ])}>
    children
  </button>;