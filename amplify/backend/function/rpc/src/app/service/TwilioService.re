let client =
  Twilio.make(Constants.Env.twilioSID, Constants.Env.twilioAuthToken);

let sendMessage = (~to_, ~body) =>
  client
  ->Twilio.Messages.resource
  ->Twilio.Messages.create(
      Twilio.Messages.createParams(
        ~from=Constants.Env.twilioPhoneNumber,
        ~to_,
        ~body,
      ),
    );
