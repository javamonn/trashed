open Lib.Styles;

let formatAge = age => {
  let date = DateFns.parseString(age);
  let now = Js.Date.make();

  let (diff, suffix) =
    switch (
      DateFns.differenceInDays(now, date) |> int_of_float,
      DateFns.differenceInHours(now, date) |> int_of_float,
      DateFns.differenceInMinutes(now, date) |> int_of_float,
    ) {
    | (dayDiff, _, _) when dayDiff > 0 => (dayDiff, "d")
    | (_, hourDiff, _) when hourDiff > 0 => (hourDiff, "h")
    | (_, _, minuteDiff) when minuteDiff > 0 => (minuteDiff, "m")
    | _ => (1, "m")
    };
  string_of_int(diff) ++ suffix;
};

[@react.component]
let make = (~item) => {
  let location = (item##location##lat, item##location##lon);
  let age = formatAge(item##createdAt);

  <ScreenBottomEdgeAligned
    className={cn(["flex", "justify-between", "py-4", "px-6"])}>
    <div className={cn(["flex", "flex-col", "justify-end"])}>
      <div
        className={cn(["flex", "flex-row", "justify-center", "items-center"])}>
        <Emoji_HourglassSVG className={cn(["w-10", "h-10"])} />
        <span
          style={style()->unsafeStyle([("-webkitTextStroke", "1.5px #000")])}
          className={cn([
            "ml-2",
            "text-brandBlue",
            "italic",
            "leading-none",
            "font-bold",
            "text-3xl",
          ])}>
          {React.string(age)}
        </span>
      </div>
    </div>
    <Map className={cn(["block", "z-10"])} location />
    <VideoSurface.ContrastOverlay height=12.5 />
  </ScreenBottomEdgeAligned>;
};
