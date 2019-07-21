let wrapBs = cb => (. a) => cb(a);

let optionalDecode = (cb, json) =>
  switch (cb(json)) {
  | v => Some(v)
  | exception (Json.Decode.DecodeError(_)) => None
  };
