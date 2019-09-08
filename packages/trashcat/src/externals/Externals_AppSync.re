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

module Graphql = {
  /** Result of graphql_ppx */
  type documentNode: {. "query": Js.Json.t };
};

[@bs.module "aws-appsync-react"]
external graphqlMutation:
  (. Graphql.documentNode, Graphql.documentNode, string, React.component('a)) =>
  React.component('b) =
  "graphqlMutation";

/**
module MakeMutation = (Config: {
  let mutationDocumentNode: Graphql.documentNode;
  let cacheUpdateDocumentNode: Graphql.documentNode;
  let typeName: string;
  let render: 
  let getMutationProp: Js.Json.t => Js.Promise.t('a);
}) => {

  [@react.component]
  let make => graphqlMutation(
    Config.mutationDocumentNode,
    Config.cacheUpdateDocumentNode,
    Config.typeName,
    (props) => props->getMutationProp
  )
    
};


**/
