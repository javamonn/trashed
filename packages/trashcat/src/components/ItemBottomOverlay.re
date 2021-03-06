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
let make = (~location, ~createdAt) => {
  let age = formatAge(createdAt);

  <div
    className={cn([
      "absolute",
      "inset-x-0",
      "bottom-0",
      "flex",
      "justify-between",
      "py-4",
      "px-6",
    ])}>
    <div
      className={cn([
        "flex",
        "flex-col",
        "justify-end",
        "items-start",
        "flex-auto",
      ])}>
      <div className={cn(["flex", "flex-row", "items-center"])}>
        <SVG
          icon=SVG.emojiHourglass
          className={cn(["w-10", "h-10"])}
          placeholderViewBox="0 0 128 128"
        />
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
  </div>;
};
