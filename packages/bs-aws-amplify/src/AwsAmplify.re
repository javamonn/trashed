/**
 * Amplify Typescript types: https://github.com/aws-amplify/amplify-js/blob/d5efee16181e108da52598f13b2eb05c15320244/packages/amazon-cognito-identity-js/index.d.ts
 */
module Config = {
  [@bs.deriving abstract]
  type cloudLogicCustom = {
    name: string,
    mutable endpoint: string,
    region: string,
  };

  [@bs.deriving abstract]
  type t = {
    [@bs.as "aws_project_region"]
    projectRegion: string,
    [@bs.as "aws_user_pools_id"]
    userPoolsId: string,
    [@bs.as "aws_user_pools_web_client_id"]
    userPoolsWebClientId: string,
    [@bs.as "aws_appsync_region"]
    appSyncRegion: string,
    [@bs.as "aws_appsync_authenticationType"]
    appSyncAuthenticationType: string,
    [@bs.as "aws_appsync_graphqlEndpoint"]
    appSyncGraphqlEndpoint: string,
    [@bs.as "aws_user_files_s3_bucket"]
    userFilesS3Bucket: string,
    [@bs.as "aws_user_files_s3_bucket_region"]
    userFilesS3BucketRegion: string,
    [@bs.as "aws_cloud_logic_custom"] [@bs.optional]
    cloudLogicCustom: array(cloudLogicCustom),
  };
  let make = t;
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

module Api = {
  type t;

  [@bs.module "@aws-amplify/api"] external inst: t = "default";

  /** graphql **/

  [@bs.deriving abstract]
  type graphqlOperation = {
    query: string,
    variables: Js.Json.t,
  };

  [@bs.send]
  external graphql: (t, graphqlOperation) => Js.Promise.t(Js.Json.t) =
    "graphql";

  /** rest **/

  [@bs.deriving abstract]
  type response('a, 'b) = {
    data: 'a,
    status: int,
    statusText: string,
    headers: Js.t('b),
  };

  [@bs.send]
  external post:
    (
      t,
      string,
      string,
      option({
        .
        "body": Js.Json.t,
        "headers": 'b,
      })
    ) =>
    Js.Promise.t(response('a, 'b)) =
    "post";

  let post = (~apiName, ~path, ~options=?, ()) =>
    post(inst, apiName, path, options);
};

type t;
[@bs.module "@aws-amplify/core"] external inst: t = "default";
[@bs.send] external configure: (t, Config.t) => unit = "configure";
