module Client = {
  type t;

  [@bs.deriving abstract]
  type authOptions = {
    [@bs.as "type"]
    type_: string,
    [@bs.optional]
    jwtToken: unit => Js.Promise.t(AwsAmplify.Auth.JwtToken.t),
    [@bs.optional]
    credentials: unit => Js.Promise.t(AwsAmplify.Auth.Credentials.t),
  };

  let authWithCognitoUserPools = (~jwtToken) =>
    authOptions(~type_="AMAZON_COGNITO_USER_POOLS", ~jwtToken, ());
  let authWithIAM = (~credentials) =>
    authOptions(~type_="AWS_IAM", ~credentials, ());

  [@bs.deriving abstract]
  type options = {
    url: string,
    region: string,
    auth: authOptions,
    disableOffline: bool,
    mandatorySignIn: bool,
    complexObjectsCredentials:
      unit => Js.Promise.t(AwsAmplify.Auth.Credentials.t),
  };

  [@bs.new] [@bs.module "aws-appsync"] external make: options => t = "default";

  external asGeneratedApolloClient: t => ApolloClient.generatedApolloClient =
    "%identity";
};

module Rehydrated = {
  [@bs.module "aws-appsync-react"] [@react.component]
  external make: (~children: React.element) => React.element = "Rehydrated";
};
