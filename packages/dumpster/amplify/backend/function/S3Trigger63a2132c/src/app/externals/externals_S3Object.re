/** s3://<bucket>/<permissionsDir>/<categoryDir>/<path> */

[@bs.deriving abstract]
type t = {
  bucket: string,
  key: string,
};
let make = t;

let primaryDirGet = o =>
  switch (Js.String.split("/", o->keyGet)->Array.to_list) {
  | [topDir, subDir, ..._rest] => topDir ++ "/" ++ subDir
  | _ => o->keyGet
  };

let toString = o => "s3://" ++ o->bucketGet ++ "/" ++ o->keyGet;
