/**
 * Amplify Typescript types: https://github.com/aws-amplify/amplify-js/blob/d5efee16181e108da52598f13b2eb05c15320244/packages/amazon-cognito-identity-js/index.d.ts
 */
module Config = {
  [@bs.deriving abstract]
  type t =
    pri {
      [@bs.as "aws_appsync_region"]
      appSyncRegion: string,
      [@bs.as "aws_appsync_authenticationType"]
      appSyncAuthenticationType: string,
      [@bs.as "aws_appsync_graphqlEndpoint"]
      appSyncGraphqlEndpoint: string,
    };
};

module Auth = {
  module JwtToken = {
    type t;
  };

  module CognitoIdToken = {
    type t;
    [@bs.send] external getJwtToken: t => JwtToken.t = "getJwtToken";
  };

  module CognitoUserSession = {
    type t;
    [@bs.send] external getIdToken: t => CognitoIdToken.t = "getIdToken";
  };

  [@bs.module "@aws-amplify/auth"]
  external currentSession: unit => Js.Promise.t(CognitoUserSession.t) =
    "currentSession";
};

[@bs.module "../aws-exports"] external config: Config.t = "default";

type t;
[@bs.module "@aws-amplify/core"] external inst: t = "default";
[@bs.send] external configure: (t, Config.t) => unit = "configure";
