module Client = {
  type t;

  [@bs.deriving abstract]
  type authOptions = {
    [@bs.as "type"]
    type_: string,
    jwtToken: unit => Js.Promise.t(Externals_Amplify.Auth.JwtToken.t),
  };

  [@bs.deriving abstract]
  type options = {
    url: string,
    region: string,
    auth: authOptions,
  };

  [@bs.new] [@bs.module "aws-appsync"] external make: options => t = "default";

  external asGeneratedApolloClient: t => ApolloClient.generatedApolloClient =
    "%identity";
};

module Rehydrated = {
  [@bs.module "aws-appsync-react"] [@react.component]
  external make: (~children: React.element) => React.element = "Rehydrated";
};
