open Externals;
open Lib.Styles;
open Lib.Utils;

OGV.setBase("static/ogv");

[@react.component]
let make = (~src) => {
  let containerRef = React.useRef(Js.Nullable.null);

  let _ =
    React.useEffect0(() => {
      let _ =
        containerRef
        ->React.Ref.current
        ->Js.Nullable.toOption
        ->Belt.Option.map(container => {
            let player = OGV.makePlayer(OGV.Player.options(~debug=true));
            let _ =
              Webapi.Dom.Element.appendChild(
                player->VideoElement.unsafeAsHtmlElement,
                container,
              );
            let _ =
              player
              ->VideoElement.setSrc(src->Js.Undefined.return)
              ->VideoElement.setMuted(true->Js.Undefined.return)
              |> doto(
                   VideoElement.addEventListener("loadedmetadata", _ev => {
                     let _ = player->VideoElement.play;
                     ();
                   }),
                 );
            Js.log(player);
          });
      None;
    });

  <div
    ref={containerRef->ReactDOMRe.Ref.domRef}
    className={cn(["w-full", "h-full", "object-cover"])}
  />;
};
