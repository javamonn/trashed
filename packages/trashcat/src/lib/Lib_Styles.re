let cn = Cn.make;

module Colors = {
  let brandYellow = "#FDE58F";
};

let style = ReactDOMRe.Style.make;

let rootFontSize = 16.0;
let rem = r => Js.Float.toString(r) ++ "rem";
let px = p => Js.Float.toString(p) ++ "px";
let remOfPx = px => rem(float_of_int(px) /. rootFontSize);

let unsafeStyle = (style, unsafeList) =>
  List.fold_left(
    (acc, (prop, value)) =>
      ReactDOMRe.Style.unsafeAddProp(acc, prop, value),
    style,
    unsafeList,
  );
