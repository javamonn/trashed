/** s3://<bucket>/<permissionsPathPart>/<categoryPathPart>/<path> */

[@bs.deriving abstract]
type t = {
  bucket: string,
  key: string,
};

let make = t;

let primaryPathPartGet = o =>
  switch (Js.String.split("/", o->keyGet)->Array.to_list) {
  | [permissionsPathPart, categoryPathPart, ..._namePathPart] =>
    Some(permissionsPathPart ++ "/" ++ categoryPathPart)
  | _ => None
  };

let namePathPartGet = o =>
  switch (Js.String.split("/", o->keyGet)->Array.to_list) {
  | [_permisssionsPathPart, _categoryPathPart, ...namePathPart] =>
    namePathPart |> Array.of_list |> Js.Array.joinWith("/") |> Js.Option.some
  | _ => None
  };

let toString = o => "s3://" ++ o->bucketGet ++ "/" ++ o->keyGet;
