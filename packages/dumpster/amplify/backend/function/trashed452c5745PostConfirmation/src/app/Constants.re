module Env = {
  [@bs.val]
  external twilioPhoneNumber: string = "process.env.TWILIO_PHONE_NUMBER";
  [@bs.val] external twilioSID: string = "process.env.TWILIO_SID";
  [@bs.val]
  external twilioAuthToken: string = "process.env.TWILIO_AUTH_TOKEN";
};

module Strings = {
  let phoneNumberRegistered = "Trashed!";
};
