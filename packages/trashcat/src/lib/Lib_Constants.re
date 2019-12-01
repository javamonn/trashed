let browser =
  Webapi.Dom.window
  ->Webapi.Dom.Window.navigator
  ->Externals_Bowser.asNavigator
  ->Externals_Bowser.userAgentGet
  ->Externals_Bowser.make
  ->Externals_Bowser.getBrowser;

module Env = {
  [@bs.val]
  external cloudfrontDistributionOrigin: string =
    "process.env.CLOUDFRONT_DISTRIBUTION_ORIGIN";
  [@bs.val] external rpcOrigin: string = "process.env.RPC_ORIGIN";
  [@bs.val] external nodeEnv: string = "process.env.NODE_ENV";
  [@bs.val]
  external serviceWorkerUrl: string = "process.env.SERVICE_WORKER_URL";
};

[@bs.module "../aws-exports"]
external awsAmplifyConfig: AwsAmplify.Config.t = "default";

awsAmplifyConfig
->AwsAmplify.Config.cloudLogicCustomGet
->Belt.Option.flatMap(c => c->Belt.Array.get(0))
->Belt.Option.map(c =>
    c->AwsAmplify.Config.endpointSet("https://" ++ Env.rpcOrigin)
  );
