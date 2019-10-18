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

  [@bs.val] external nodeEnv: string = "process.env.NODE_ENV";
};

[@bs.module "../aws-exports"]
external awsAmplifyConfig: AwsAmplify.Config.t = "default";
