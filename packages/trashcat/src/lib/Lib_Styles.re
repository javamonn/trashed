let cn = Cn.make;

module Colors = {
  let brandYellow = "#FDE58F";
};

let style = ReactDOMRe.Style.make;

let rootFontSize = 16.0;
let rem = r => string_of_int(r) ++ "rem";
let rem = r => string_of_float(r) ++ "rem";
let remOfPx = px => rem(float_of_int(px) /. rootFontSize);

let unsafeStyle = (style, unsafeList) =>
  List.fold_left(
    (acc, (prop, value)) =>
      ReactDOMRe.Style.unsafeAddProp(acc, prop, value),
    style,
    unsafeList,
  );
