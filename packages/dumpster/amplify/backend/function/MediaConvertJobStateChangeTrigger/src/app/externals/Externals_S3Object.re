[@bs.deriving abstract]
type t = {
  bucket: string,
  key: string,
};

let make = t;

let fromString = s =>
  switch (
    s
    |> Js.String.sliceToEnd(~from=5)
    |> Js.String.split("/")
    |> Array.to_list
  ) {
  | [bucket, ...keyPart] =>
    Some(
      make(~bucket, ~key=keyPart |> Array.of_list |> Js.Array.joinWith("/")),
    )
  | _ => None
  };
