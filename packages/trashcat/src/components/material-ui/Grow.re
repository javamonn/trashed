[@bs.module "@material-ui/core/Grow"] [@react.component]
external make:
  (
    ~_in: bool,
    ~appear: bool,
    ~onExited: Dom.htmlElement => unit=?,
    ~children: React.element
  ) =>
  React.element =
  "default";
