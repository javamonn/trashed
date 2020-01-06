/** s3://<bucket>/<permissionsPathPart>/<categoryPathPart>/<path> */

[@bs.deriving accessors]
type t = {
  bucket: string,
  key: string,
  mimeType: option(string)
};

let primaryPathPartGet = o =>
  switch (Js.String.split("/", o->key)->Array.to_list) {
  | [permissionsPathPart, categoryPathPart, ..._namePathPart] =>
    Some(permissionsPathPart ++ "/" ++ categoryPathPart)
  | _ => None
  };

let namePathPartGet = o =>
  switch (Js.String.split("/", o->key)->Array.to_list) {
  | [_permisssionsPathPart, _categoryPathPart, ...namePathPart] =>
    namePathPart |> Array.of_list |> Js.Array.joinWith("/") |> Js.Option.some
  | _ => None
  };

let toString = o => "s3://" ++ o->bucket ++ "/" ++ o->key;
