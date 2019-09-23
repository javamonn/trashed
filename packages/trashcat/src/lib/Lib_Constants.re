let browser =
  Webapi.Dom.window
  ->Webapi.Dom.Window.navigator
  ->Externals_Bowser.asNavigator
  ->Externals_Bowser.userAgentGet
  ->Externals_Bowser.make
  ->Externals_Bowser.getBrowser;

