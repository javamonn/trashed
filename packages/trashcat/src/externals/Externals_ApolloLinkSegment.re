type analytics = {track: (string, Js.Json.t) => unit};

type options = {
  analytics,
  operationWhitelist: option(array(string)),
};

[@bs.val] [@bs.module "apollo-link-segment"]
external make: options => ReasonApolloTypes.apolloLink = "default"
