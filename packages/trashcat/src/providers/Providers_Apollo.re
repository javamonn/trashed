open Externals;
open Lib;

let authenticatedClientAuthOptions =
  AppSync.Client.authWithCognitoUserPools(~jwtToken=() =>
    AwsAmplify.(
      Auth.(inst->currentSession)
      |> Js.Promise.then_(s =>
           s
           ->Auth.CognitoUserSession.getIdToken
           ->Auth.CognitoIdToken.getJwtToken
           ->Js.Promise.resolve
         )
    )
  );

let unauthenticatedClientAuthOptions =
  AppSync.Client.authWithIAM(~credentials=() =>
    AwsAmplify.Auth.(inst->currentCredentials)
  );

/**
 * FIXME: https://github.com/awslabs/aws-mobile-appsync-sdk-js/issues/450#issuecomment-522264166
 */
let clientOptions =
  AppSync.Client.options(
    ~url=
      AwsAmplify.(
        Constants.awsAmplifyConfig->Config.appSyncGraphqlEndpointGet
      ),
    ~region=AwsAmplify.(Constants.awsAmplifyConfig->Config.appSyncRegionGet),
    ~auth=unauthenticatedClientAuthOptions,
    ~disableOffline=true,
    ~complexObjectsCredentials=
      () => AwsAmplify.Auth.(inst->currentCredentials),
    ~mandatorySignIn=false,
  );

let client = AppSync.Client.make(clientOptions);

[@react.component]
let make = (~children) =>
  <ReasonApollo.Provider
    client={client->AppSync.Client.asGeneratedApolloClient}>
    <AppSync.Rehydrated>
      <ApolloHooks.Provider
        client={client->AppSync.Client.asGeneratedApolloClient}>
        children
      </ApolloHooks.Provider>
    </AppSync.Rehydrated>
  </ReasonApollo.Provider>;
