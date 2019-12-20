open Lib.Styles;

type direction =
  | Horizontal
  | Vertical;

module Container = {
  [@react.component]
  let make = (~direction, ~onIdxChange=?, ~initialIdx=?, ~children) => {
    let containerRef = React.useRef(Js.Nullable.null);
    let (hasScrolledToInitialIdx, setHasScrolledToInitialIdx) =
      React.useState(() =>
        switch (initialIdx) {
        | Some(idx) when idx === 0 => true
        | Some(_)
        | None => false
        }
      );
    let _ =
      React.useEffect0(() => {
        let _ =
          switch (initialIdx) {
          | Some(initialIdx) when initialIdx > 0 && !hasScrolledToInitialIdx =>
            let elem = containerRef->React.Ref.current->Js.Nullable.toOption;
            switch (elem, direction) {
            | (Some(elem), Horizontal) =>
              let width = Webapi.Dom.Element.clientWidth(elem);
              let _ =
                Webapi.Dom.Element.setScrollLeft(
                  elem,
                  float_of_int(width * initialIdx),
                );
              let _ = setHasScrolledToInitialIdx(_ => true);
              ();
            | (Some(elem), Vertical) =>
              let height = Webapi.Dom.Element.clientHeight(elem);
              let _ =
                Webapi.Dom.Element.setScrollTop(
                  elem,
                  float_of_int(height * initialIdx),
                );
              let _ = setHasScrolledToInitialIdx(_ => true);
              ();
            | _ => ()
            };
          | _ => ()
          };
        None;
      });

    let onScroll = ev => {
      let _ = ReactEvent.UI.stopPropagation(ev);
      let (scroll, containerDimen) =
        switch (direction) {
        | Vertical =>
          let scrollTop = int_of_float(ReactEvent.UI.target(ev)##scrollTop);
          let bodyHeight =
            Webapi.Dom.(
              document
              |> Document.unsafeAsHtmlDocument
              |> HtmlDocument.body
              |> Js.Option.getExn
              |> Element.clientHeight
            );
          (scrollTop, bodyHeight);
        | Horizontal =>
          let scrollLeft =
            int_of_float(ReactEvent.UI.target(ev)##scrollLeft);
          let windowWidth = Webapi.Dom.(window->Window.innerWidth);
          (scrollLeft, windowWidth);
        };
      let activeIdx =
        children
        ->Belt.Array.mapWithIndex((idx, _item) => idx * containerDimen)
        ->Belt.Array.getIndexBy(itemPos => scroll === itemPos);
      let _ =
        switch (activeIdx, onIdxChange) {
        | (Some(activeIdx), Some(onIdxChange)) =>
          onIdxChange(activeIdx)
        | _ => ()
        };
      ();
    };

    <div
      onScroll=?{hasScrolledToInitialIdx ? Some(onScroll) : None}
      ref={containerRef->ReactDOMRe.Ref.domRef}
      className={cn([
        "h-screen",
        "w-screen",
        "invisible"->Cn.ifTrue(!hasScrolledToInitialIdx),
        switch (direction) {
        | Horizontal => cn(["overflow-x-scroll", "flex"])
        | Vertical => cn(["overflow-y-scroll"])
        },
      ])}
      style={
        style()
        ->unsafeStyle([
            (
              "scrollSnapType",
              switch (direction) {
              | Horizontal => "x mandatory"
              | Vertical => "y mandatory"
              },
            ),
            ("scrollSnapStop", "always"),
          ])
      }>
      {React.array(children)}
    </div>;
  };
};

module Item = {
  [@react.component]
  let make = (~className=?, ~direction, ~children=React.null) => {
    <div
      style={
        style()
        ->unsafeStyle([
            ("scrollSnapStop", "always"),
            ("scrollSnapAlign", "start"),
          ])
      }
      className={cn([
        "w-screen",
        "h-screen",
        switch (direction) {
        | Horizontal => cn(["min-w-full"])
        | Vertical => ""
        },
        className->Cn.unpack,
      ])}>
      children
    </div>;
  };
};
