/* Amplify Params - DO NOT EDIT
You can access the following resource attributes as environment variables from your Lambda function
var environment = process.env.ENV
var region = process.env.REGION
var authTrashed452c5745UserPoolId = process.env.AUTH_TRASHED452C5745_USERPOOLID
var apiTrashedGraphQLAPIIdOutput = process.env.API_TRASHED_GRAPHQLAPIIDOUTPUT
var apiTrashedGraphQLAPIEndpointOutput = process.env.API_TRASHED_GRAPHQLAPIENDPOINTOUTPUT
var storageTrashcatBucketName = process.env.STORAGE_TRASHCAT_BUCKETNAME

Amplify Params - DO NOT EDIT */

const app = require('./app/Index.bs.js')

exports.handler = function (event, context, cb) { 
  app.handler(event, context, cb)
};
