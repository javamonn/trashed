/**
 * Mobile Chrome and Safari both have collapsable address bar chrome, but
 * vh units are agnostic to it. We workaround by "filling available" space,
 * resulting in the app occupying the remaining space with the address bar
 * remaining expanded.
 *
 * This makes sense for screens we want to be full screen,
 * but may be worth revising in context of screens that don't have strict
 * layout constraints (e.g. blog, about, text, etc)
 */

let getVisibleHeight = () => Webapi.Dom.(window |> Window.innerHeight);

let getScreenHeight = () =>
  Webapi.Dom.(
    document
    |> Document.unsafeAsHtmlDocument
    |> HtmlDocument.body
    |> Js.Option.getExn
    |> Element.clientHeight
  );

let calculateRootHeight = () => {
  let offsetHeight = getScreenHeight() - getVisibleHeight();
  let rootHeight = "calc(100vh - " ++ string_of_int(offsetHeight) ++ "px)";
  let _ =
    Webapi.Dom.(
      document
      |> Document.documentElement
      |> Element.unsafeAsHtmlElement
      |> HtmlElement.style
      |> CssStyleDeclaration.setProperty("--root-height", rootHeight, "")
    );
  ();
};

let make = () => {
  let onResize = _ev => {
    calculateRootHeight();
  };
  let _ = Webapi.Dom.(window |> Window.addEventListener("resize", onResize));
  let _ = calculateRootHeight();

  () => {
    let _ =
      Webapi.Dom.(window |> Window.removeEventListener("resize", onResize));
    ();
  };
};
