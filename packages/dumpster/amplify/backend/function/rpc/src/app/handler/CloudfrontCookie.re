open Lib;

let signer =
  AWSSDK.CloudFront.Signer.make(
    Constants.Env.cloudfrontKeyPairId,
    Constants.Env.cloudfrontPrivateKey,
  );

let makePolicy = timestamp =>
  Json.Encode.(
    object_([
      (
        "Statement",
        jsonArray([|
          object_([
            (
              "Resource",
              string(
                "https://"
                ++ Constants.Env.cloudfrontDistributionOrigin
                ++ "/public/*",
              ),
            ),
            (
              "Condition",
              object_([
                (
                  "DateLessThan",
                  object_([("AWS:EpochTime", timestamp->int)]),
                ),
              ]),
            ),
          ]),
        |]),
      ),
    ])
  )
  ->Js.Json.stringify;

let handler =
  Express.Middleware.from((_next, _request, response) => {
    let ms = Js.Date.make() |> DateFns.addDays(14.) |> Js.Date.getTime;
    let policy = makePolicy(int_of_float(ms /. 1000.));
    let signedCookie =
      signer->AWSSDK.CloudFront.Signer.getSignedCookie({
        "policy": policy |> Js.Option.some,
        "expires": None,
        "url": None,
      });
    Express.Response.(
      response
      |> cookie(
           ~name="CloudFront-Policy",
           ~domain=Constants.Env.domain,
           ~maxAge=60*60*24*7,
           signedCookie
           ->AWSSDK.CloudFront.Signer.SignedCookie.policyGet
           ->Json.Encode.string,
         )
      |> cookie(
           ~name="CloudFront-Signature",
           ~domain=Constants.Env.domain,
           ~maxAge=60*60*24*7,
           signedCookie
           ->AWSSDK.CloudFront.Signer.SignedCookie.signatureGet
           ->Json.Encode.string,
         )
      |> cookie(
           ~name="CloudFront-Key-Pair-Id",
           ~domain=Constants.Env.domain,
           ~maxAge=60*60*24*7,
           signedCookie
           ->AWSSDK.CloudFront.Signer.SignedCookie.keyPairIdGet
           ->Json.Encode.string,
         )
      |> status(StatusCode.Ok)
      |> sendString("Ok.")
    );
  });
