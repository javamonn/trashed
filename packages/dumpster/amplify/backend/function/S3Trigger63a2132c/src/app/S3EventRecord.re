[@bs.deriving abstract]
type bucket = {name: string};

[@bs.deriving abstract]
type object_ = {key: string};

let primaryDir = key =>
  switch (Js.String.split("/", key)->Array.to_list) {
  | [topDir, subDir, ..._rest] => topDir ++ "/" ++ subDir
  | _ => key
  };

[@bs.deriving abstract]
type s3 = {
  bucket,
  [@bs.as "object"]
  object_,
};

[@bs.deriving abstract]
type t = {
  eventName: string,
  s3,
};
