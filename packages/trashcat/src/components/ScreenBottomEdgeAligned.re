open Lib.Styles;

[@react.component]
let make = (~className=?, ~children) => {
  let (visibleHeight, setVisibleHeight) =
    React.useState(() => Webapi.Dom.(window |> Window.innerHeight));

  let _ =
    React.useEffect0(() => {
      let handleResize = _ev => {
        let _ =
          setVisibleHeight(_ => Webapi.Dom.(window |> Window.innerHeight));
        ();
      };
      let _ =
        Webapi.Dom.(
          window |> Window.addEventListener("resize", handleResize)
        );
      Some(
        () =>
          Webapi.Dom.(
            window |> Window.removeEventListener("resize", handleResize)
          ),
      );
    });

  let screenHeight =
    Webapi.Dom.(
      document
      |> Document.unsafeAsHtmlDocument
      |> HtmlDocument.body
      |> Js.Option.getExn
      |> Element.clientHeight
    );
  let bottomOffset = float_of_int(screenHeight - visibleHeight);

  <div
    className={cn(["absolute", "inset-x-0", className->Cn.unpack])}
    style={style(~bottom=px(bottomOffset), ())}>
    children
  </div>;
};
