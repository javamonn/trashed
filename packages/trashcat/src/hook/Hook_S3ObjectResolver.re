open Lib;

open Externals;

module S3Resolver = {
  let resolve = s3Object =>
    AwsAmplify.Storage.(inst->get(s3Object->S3Object.keyGet));
};

module CloudFrontResolver = {
  type authenticationState =
    | Unauthenticated
    | AuthenticationInProgress(Js.Promise.t(unit))
    | Authenticated;

  let authenticationState = ref(Unauthenticated);

  let authenticate = () =>
    switch (authenticationState^) {
    | Unauthenticated =>
      let p =
        AwsAmplify.Api.post(
          ~apiName="rpc",
          ~path="/authentication/cloudfront-cookie",
          (),
        )
        |> Js.Promise.then_(_response => Js.Promise.resolve());
      authenticationState := AuthenticationInProgress(p);
      p;
    | AuthenticationInProgress(p) => p
    | Authenticated => Js.Promise.resolve()
    };

  let resolve = s3Object => {
    let protocol = Webapi.Dom.location->Webapi.Dom.Location.protocol;
    let origin = Constants.Env.cloudfrontDistributionOrigin;
    let key = s3Object->S3Object.keyGet;

    authenticate()
    |> Js.Promise.then_(() =>
         {j|$protocol//$origin/$key|j}->Js.Promise.resolve
       );
  };
};

module State = {
  [@bs.deriving accessors]
  type t('a) =
    | Resolving
    | Resolved('a)
    | Error;

  [@bs.deriving accessors]
  type action('a) =
    | SetState(t('a));
};

let use = s3Object => {
  let (state, dispatch) =
    React.useReducer(
      (_state, action) =>
        switch (action) {
        | State.SetState(s) => s
        },
      State.Resolving,
    );

  let resolver =
    Constants.Env.nodeEnv === "production"
      ? CloudFrontResolver.resolve : S3Resolver.resolve;

  let _ =
    React.useEffect1(
      () => {
        let _ =
          s3Object->Belt.Option.map(o =>
            o
            |> resolver
            |> Js.Promise.then_(r => {
                 let _ = r->State.resolved->State.setState->dispatch;
                 Js.Promise.resolve();
               })
            |> Js.Promise.catch(err => {
                 Js.log(err);
                 let _ = State.error->State.setState->dispatch;
                 Js.Promise.resolve();
               })
          );
        None;
      },
      switch (s3Object) {
      | Some(d) => [|d|]
      | None => [||]
      },
    );
  state;
};

let useMany = s3Objects => {
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
          s3Objects->Belt.Option.map(a =>
            a
            |> Js.Array.map(
                 Constants.Env.nodeEnv === "production"
                   ? CloudFrontResolver.resolve : S3Resolver.resolve,
               )
            |> Js.Promise.all
            |> Js.Promise.then_(r => {
                 let _ = r->State.resolved->State.setState->dispatch;
                 Js.Promise.resolve();
               })
            |> Js.Promise.catch(err => {
                 Js.log(err);
                 let _ = State.error->State.setState->dispatch;
                 Js.Promise.resolve();
               })
          );
        None;
      },
      s3Objects->Belt.Option.getWithDefault([||]),
    );
  state;
};
