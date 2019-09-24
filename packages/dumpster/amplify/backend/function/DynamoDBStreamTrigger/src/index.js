/* Amplify Params - DO NOT EDIT
You can access the following resource attributes as environment variables from your Lambda function
var environment = process.env.ENV
var region = process.env.REGION
var apiTrashedGraphQLAPIIdOutput = process.env.API_TRASHED_GRAPHQLAPIIDOUTPUT
var apiTrashedGraphQLAPIEndpointOutput = process.env.API_TRASHED_GRAPHQLAPIENDPOINTOUTPUT
var storageTrashcatBucketName = process.env.STORAGE_TRASHCAT_BUCKETNAME

Amplify Params - DO NOT EDIT */

exports.handler = function (event, context) { //eslint-disable-line
  console.log(`value1 = ${event.key1}`);
  console.log(`value2 = ${event.key2}`);
  console.log(`value3 = ${event.key3}`);
  context.done(null, 'Hello World'); // SUCCESS with message
};
