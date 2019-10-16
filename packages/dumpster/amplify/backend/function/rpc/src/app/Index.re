/* Amplify Params - DO NOT EDIT
   You can access the following resource attributes as environment variables from your Lambda function
   var environment = process.env.ENV
   var region = process.env.REGION
   var authTrashed452c5745UserPoolId = process.env.AUTH_TRASHED452C5745_USERPOOLID
   var apiTrashedGraphQLAPIIdOutput = process.env.API_TRASHED_GRAPHQLAPIIDOUTPUT
   var apiTrashedGraphQLAPIEndpointOutput = process.env.API_TRASHED_GRAPHQLAPIENDPOINTOUTPUT

   Amplify Params - DO NOT EDIT */

let express = Express.App.make();

let _ =
  express->(
             Express.App.useWithMany([|
               ParseBodyAsJsonMiddleware.make(),
               AWSServerlessExpressMiddleware.make(),
               CORSMiddleware.make(),
             |])
           );
let _ =
  express->(
             Express.App.post(
               ~path="/authentication/cloudfront-cookie",
               CloudfrontCookie.handler,
             )
           );

let _ = express->(Express.App.use(PathNotFoundMiddleware.make()));

let default = express;
