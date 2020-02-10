open Externals;
open Lib;
open Service;

let authenticatedClientAuthOptions =
  AwsAppSync.Client.authWithCognitoUserPools(~jwtToken=() =>
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
  AwsAppSync.Client.authWithIAM(~credentials=() =>
    AwsAmplify.Auth.(inst->currentCredentials)
  );

/**
 * FIXME: https://github.com/awslabs/aws-mobile-appsync-sdk-js/issues/450#issuecomment-522264166
 */

let appSyncLinkOptions =
  AwsAppSync.Client.{
    url:
      AwsAmplify.(
        Constants.awsAmplifyConfig->Config.appSyncGraphqlEndpointGet
      ),
    region: AwsAmplify.(Constants.awsAmplifyConfig->Config.appSyncRegionGet),
    auth: unauthenticatedClientAuthOptions,
    disableOffline: true,
    complexObjectsCredentials: () =>
      AwsAmplify.Auth.(inst->currentCredentials),
    mandatorySignIn: false,
  };
let appSyncLink = AwsAppSync.Client.createAppSyncLink(appSyncLinkOptions);
let analyticsLink =
  ApolloLinkAnalytics.make((operation, _data) => {
    Analytics.(
      {
        operationName: operation->ApolloLink.operationName,
        variables: operation->ApolloLink.variables,
      }
      ->graphQLOperation
      ->track
    )
  });

let client =
  AwsAppSync.Client.(
    makeWithOptions(
      appSyncLinkOptions,
      {link: ApolloLinks.from([|analyticsLink, appSyncLink|])},
    )
  );

[@react.component]
let make = (~children) =>
  <ReasonApollo.Provider client>
    <AwsAppSync.Rehydrated>
      <ApolloHooks.Provider client> children </ApolloHooks.Provider>
    </AwsAppSync.Rehydrated>
  </ReasonApollo.Provider>;
