let flip2 = (f, a, b) => f(b, a);
let flip3 = (f, a, b, c) => f(b, c, a);

let ( *> ) = (x, f) => {
  let _ = f(x);
  x;
};

/** target is the first arg of the _wrapped_ function */
let doto = (f, inst) => {
  let _ = f(inst);
  inst;
};
let doto1 = (f, inst, arg1) => {
  let _ = f(inst, arg1);
  inst;
};

let wrapBs = (a, f) => f(. a);

let apply = (f, a) => f(a);

let identity = x => x;

module Pair = {
  let fromArray = a =>
    switch (Array.to_list(a)) {
    | [k, v] => Some((k, v))
    | _ => None
    };
};
