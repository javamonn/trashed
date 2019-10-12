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

let toString = o => "s3://" ++ o->bucketGet ++ "/" ++ o->keyGet;

let toObjectInput = o => {
  "key": o->keyGet,
  "bucket": o->bucketGet,
  "region": Lib_Constants.Env.region,
  "localUri": None,
  "mimeType": None,
};

[@bs.deriving jsConverter]
type mimeType = [
  | [@bs.as "WEBM"] `WEBM
  | [@bs.as "MP4"] `MP4
  | [@bs.as "JPEG"] `JPEG
];
let mimeTypeGet = o =>
  switch (o->toString->Externals_Path.extName) {
  | ".mp4" => Some(`MP4)
  | ".webm" => Some(`WEBM)
  | ".jpg" => Some(`JPEG)
  | _ => None
  };
