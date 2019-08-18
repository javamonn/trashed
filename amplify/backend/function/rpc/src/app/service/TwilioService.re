let client =
  TwilioBinding.make(Constants.Env.twilioSID, Constants.Env.twilioAuthToken);

let sendMessage = (~to_, ~body) =>
  client
  ->TwilioBinding.Messages.resource
  ->TwilioBinding.Messages.create(
      TwilioBinding.Messages.createParams(
        ~from=Constants.Env.twilioPhoneNumber,
        ~to_,
        ~body,
      ),
    );
