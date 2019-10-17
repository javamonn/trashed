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
        object_([
          (
            "Resource",
            string(
              "https//"
              ++ Constants.Env.cloudfrontDistributionOrigin
              ++ "/public/*",
            ),
          ),
          ("Condition", object_([("AWS:EpochTime", timestamp->float)])),
        ]),
      ),
    ])
  )
  ->Js.Json.stringify;

let handler =
  Express.PromiseMiddleware.from((_next, request, response) =>
    signer
    ->AWSSDK.CloudFront.Signer.getSignedCookie({
        "policy":
          Js.Date.make()
          |> DateFns.addDays(14.)
          |> Js.Date.getTime
          |> makePolicy
          |> Js.Option.some,
        "expires": None,
        "url": None,
      })
    ->Utils.Promise.then_(signedCookie =>
        Express.Response.(
          response
          |> cookie(
               ~name="CloudFront-Policy",
               ~secure=true,
               ~httpOnly=true,
               signedCookie
               ->AWSSDK.CloudFront.Signer.SignedCookie.policyGet
               ->Json.Encode.string,
             )
          |> cookie(
               ~name="CloudFront-Signature",
               ~secure=true,
               ~httpOnly=true,
               signedCookie
               ->AWSSDK.CloudFront.Signer.SignedCookie.signatureGet
               ->Json.Encode.string,
             )
          |> cookie(
               ~name="CloudFront-Key-Pair-Id",
               ~secure=true,
               ~httpOnly=true,
               signedCookie
               ->AWSSDK.CloudFront.Signer.SignedCookie.keyPairIdGet
               ->Json.Encode.string,
             )
          |> status(StatusCode.Ok)
          |> sendString("Ok.")
          |> Js.Promise.resolve
        )
      )
  );
