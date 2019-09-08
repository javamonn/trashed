open Externals;

let clientAuth =
  AppSync.Client.authOptions(
    ~type_=Amplify.(config->Config.appSyncAuthenticationTypeGet), ~jwtToken=() =>
    Amplify.(
      Auth.currentSession()
      |> Js.Promise.then_(s =>
           s
           ->Auth.CognitoUserSession.getIdToken
           ->Auth.CognitoIdToken.getJwtToken
           ->Js.Promise.resolve
         )
    )
  );
let clientOptions =
  AppSync.Client.options(
    ~url=Amplify.(config->Config.appSyncGraphqlEndpointGet),
    ~region=Amplify.(config->Config.appSyncRegionGet),
    ~auth=clientAuth,
  );

let client = AppSync.Client.make(clientOptions);

[@react.component]
let make = (~children) =>
  <ReasonApollo.Provider
    client={client->AppSync.Client.asGeneratedApolloClient}>
    <ReasonApolloHooks.ApolloProvider
      client={client->AppSync.Client.asGeneratedApolloClient}>
      children
    </ReasonApolloHooks.ApolloProvider>
  </ReasonApollo.Provider>;
