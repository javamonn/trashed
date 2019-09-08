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

  [@bs.new] [@bs.module] external make: options => t = "aws-appsync";

  external asGeneratedApolloClient: t => ApolloClient.generatedApolloClient =
    "%identity";
};
