open Lib.Styles;

type direction =
  | Horizontal
  | Vertical;

module Container = {
  [@react.component]
  let make = (~direction, ~onScroll=?, ~initialIdx=?, ~children) => {
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

    <div
      onScroll=?{hasScrolledToInitialIdx ? onScroll : None}
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
      children
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
