[@bs.val] [@bs.scope ("window", "localStorage")]
external getItem: string => option(string) = "getItem";

[@bs.val] [@bs.scope ("window", "localStorage")]
external setItem: (string, string) => unit = "setItem";
