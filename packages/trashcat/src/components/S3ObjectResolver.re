open Externals;

module S3Object = {
  [@bs.deriving abstract]
  type t = {
    bucket: string,
    key: string,
    region: string,
  };

  let make = t;

  let fromJs = js =>
    make(~bucket=js##bucket, ~key=js##key, ~region=js##region);

  /**
   * Storage.get expects a key without the explicit level prefix (public,
   * private, protected).
   */
  let storageKeyGet = inst =>
    switch (Js.String.split("/", inst->keyGet)) {
    | [|"public", key|] => key
    | _ => inst->keyGet
    };
};

module State = {
  [@bs.deriving accessors]
  type t =
    | Resolving
    | Resolved(string)
    | Error;

  [@bs.deriving accessors]
  type action =
    | SetState(t);
};

/** TODO: renderLoader callback */

[@react.component]
let make = (~s3Object, ~render) => {
  let (state, dispatch) =
    React.useReducer(
      (_state, action) =>
        switch (action) {
        | State.SetState(s) => s
        },
      State.Resolving,
    );

  let _ =
    React.useEffect1(
      () => {
        let _ =
          Amplify.Storage.(inst->get(s3Object->S3Object.storageKeyGet))
          |> Js.Promise.then_(r => {
               let _ = r->State.resolved->State.setState->dispatch;
               Js.Promise.resolve();
             })
          |> Js.Promise.catch(err => {
               Js.log(err);
               let _ = State.error->State.setState->dispatch;
               Js.Promise.resolve();
             });
        None;
      },
      [|s3Object|],
    );

  switch (state) {
  | Resolving
  | Error => React.null
  | Resolved(url) => render(~url)
  };
};