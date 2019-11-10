open Lib.Styles;

[@react.component]
let make = () => {
  <div
    className={cn([
      "w-screen",
      "h-screen",
      "bg-brandBlue",
      "flex",
      "justify-center",
      "items-center",
      "flex-col",
      "relative",
    ])}>
    {ReactDOMRe.createElementVariadic(
       "marquee",
       ~props=
         ReactDOMRe.objToDOMProps({
           "class":
             cn([
               "text-white",
               "font-bold",
               "italic",
               "uppercase",
               "text-6xl",
               "leading-none",
               "text-center",
               "absolute",
               "top-0",
               "left-0",
               "right-0",
             ]),
           "scrollamount": 20,
         }),
       [|React.string("Coming Soon to NYC")|],
     )}
    <div className={cn(["my-32", "h-56", "w-56"])}>
      <TrashedIconOnBackgroundSVG
        className={cn(["w-fill", "h-fill"])}
        backgroundColor="#FDE58F"
      />
    </div>
    {ReactDOMRe.createElementVariadic(
       "marquee",
       ~props=
         ReactDOMRe.objToDOMProps({
           "class":
             cn([
               "text-white",
               "font-bold",
               "italic",
               "uppercase",
               "text-6xl",
               "leading-none",
               "text-center",
               "absolute",
               "bottom-0",
               "left-0",
               "right-0",
             ]),
           "direction": "right",
           "scrollamount": 20,
         }),
       [|React.string("Coming Soon to NYC")|],
     )}
  </div>;
};
