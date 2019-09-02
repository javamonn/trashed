open Externals;
open Lib.Styles;

module HtmlInputElement = {
  type t = Webapi.Dom.HtmlInputElement.t;
  external ofHtmlElement: Webapi.Dom.HtmlElement.t => t = "%identity";
};

[@react.component]
let make = (~mimeType, ~onFile) => {
  let inputRef = React.useRef(Js.Nullable.null);
  let proxyClick = () => {
    let _ =
      inputRef
      ->React.Ref.current
      ->Js.Nullable.toOption
      ->Belt.Option.flatMap(Webapi.Dom.HtmlElement.ofElement)
      ->Belt.Option.map(Webapi.Dom.HtmlElement.click);
    ();
  };
  let _ =
    React.useEffect0(() => {
      proxyClick();
      None;
    });

  let handleChange = _ev => {
    inputRef
    ->React.Ref.current
    ->Js.Nullable.toOption
    ->Belt.Option.map(elem => {
        let _ =
          elem
          ->Webapi.Dom.HtmlElement.ofElement
          ->Belt.Option.map(HtmlInputElement.ofHtmlElement)
          ->Belt.Option.map(Webapi.Dom.HtmlInputElement.value)
          ->Belt.Option.map(File.stringToFile)
          ->Belt.Option.map(onFile);
        ();
      });
  };

  let input =
    ReactDOMRe.createElementVariadic(
      "input",
      ~props=
        ReactDOMRe.objToDOMProps({
          "type": "file",
          "accept": "video/*",
          "onChange": handleChange,
          "capture": "environment",
          "ref": inputRef->ReactDOMRe.Ref.domRef,
        }),
      [||],
    );
  <div
    onClick={_ev => proxyClick()}
    className={cn(["w-screen", "h-screen", "opacity-0"])}>
    input
  </div>;
};
