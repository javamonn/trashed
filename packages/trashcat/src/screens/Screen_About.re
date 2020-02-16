open Lib.Styles;

let points = [|
  "trashed is like the \"free stuff\" section on craigslist, but for things that have already been thrown out.",
  "anyone can post anything on trashed, as long as it\'s free and in a public place like a curb, sidewalk, or dumpster.",
  "posts on trashed are short videos of the thing and include tags (what it is) and location (where it is).",
  "others can find available things by searching posts by tag or location.",
  "anyone can pick anything up at any time for free.",
|];

[@react.component]
let make = () =>
  <div
    className={cn([
      "w-full",
      "h-full",
      "flex",
      "flex-col",
      "bg-brandYellow",
      "py-8",
      "px-6",
    ])}>
    <div className={cn(["py-8", "text-xl", "font-bold"])}>
      {React.string(
         "trashed is a free, decentralized, and efficient peer-to-peer recycling system.",
       )}
    </div>
    {points
     ->Belt.Array.map(text =>
         <div className={cn(["flex", "flex-row", "my-4", "items-center"])}>
           <div
             className={cn([
               "border-black",
               "border-4",
               "w-8",
               "h-8",
               "mr-6",
               "flex-shrink-0",
             ])}
           />
           <div className={cn(["text-black", "text-xl", "font-bold"])}>
             {React.string(text)}
           </div>
         </div>
       )
     ->React.array}
  </div>;
