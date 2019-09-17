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
      [@bs.as "aws_user_files_s3_bucket"]
      s3ItemVideoUploadBucket: string,
      [@bs.as "aws_user_files_s3_bucket_region"]
      s3ItemVideoUploadBucketRegion: string,
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

  module Credentials = {
    type t;
  };

  type t;

  [@bs.module "@aws-amplify/auth"] external inst: t = "default";

  [@bs.send]
  external currentSession: t => Js.Promise.t(CognitoUserSession.t) =
    "currentSession";

  [@bs.send]
  external currentCredentials: t => Js.Promise.t(Credentials.t) =
    "currentCredentials";
};

module Storage = {
  type t;

  [@bs.module "@aws-amplify/storage"] external inst: t = "default";

  [@bs.send] external get: (t, string) => Js.Promise.t(string) = "get";
};

[@bs.module "../aws-exports"] external config: Config.t = "default";

type t;
[@bs.module "@aws-amplify/core"] external inst: t = "default";
[@bs.send] external configure: (t, Config.t) => unit = "configure";
