let flip2 = (f, a, b) => f(b, a);

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

external unsafeAsJsObj: 'a => Js.t('b) = "%identity";
