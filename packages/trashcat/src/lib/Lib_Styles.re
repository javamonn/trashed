let cn = Cn.make;

module Colors = {
  let brandYellow = "#FDE58F";
};

let style = ReactDOMRe.Style.make;
let unsafeStyle = (style, unsafeList) =>
  List.fold_left(
    (acc, (prop, value)) =>
      ReactDOMRe.Style.unsafeAddProp(acc, prop, value),
    style,
    unsafeList,
  );
