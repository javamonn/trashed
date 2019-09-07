module PrimaryText = {
  [@react.component]
  let make = (~className=?, ~children) =>
    <span
      className={Cn.make(["text-primary", "text-lg", className->Cn.unpack])}>
      children
    </span>;
};
