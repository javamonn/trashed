open Lib;
open Service;

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
      authenticationState :=
        AuthenticationInProgress(RPC.Authentication.cloudfrontCookie());
      authenticationState^;
    | AuthenticationInProgress(p) => p
    | Authenticated => Js.Promise.resolve()
    };

  let resolve = s3Object => {
    let protocol = Webapi.Dom.location->Webapi.Dom.Location.protocol;
    let origin = Constants.Env.cloudfrontDistributionOrigin;
    let key = s3Object->S3Object.keyGet;

    authenticate()
    |> Js.Promise.then_(() =>
         {j|$protocal//$origin/$key|j}->Js.Promise.resolve
       );
  };
};

module State = {
  [@bs.deriving accessors]
  type t =
    | Resolving
    | Resolved(array(string))
    | Error;

  [@bs.deriving accessors]
  type action =
    | SetState(t);
};

let make = s3Object => {
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
          s3Object
          |> Js.Array.map(
               Constants.Env.nodeEnv === "production"
                 ? resolveFromCloudfront : resolveFromS3,
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
             });
        None;
      },
      [|s3Object|],
    );
  state;
};
