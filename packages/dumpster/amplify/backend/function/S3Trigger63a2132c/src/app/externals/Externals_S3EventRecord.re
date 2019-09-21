[@bs.deriving abstract]
type bucket = {name: string};

[@bs.deriving abstract]
type object_ = {key: string};

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
