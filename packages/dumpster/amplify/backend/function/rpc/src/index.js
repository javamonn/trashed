if (process.env.NODE_ENV !== 'production') {
  require('dotenv').config({debug: true, path: '.env'});
}

const awsServerlessExpress = require('aws-serverless-express');
const app = require('./app/Index.bs.js').default;

const server = awsServerlessExpress.createServer(app);

exports.handler = (event, context) => {
  awsServerlessExpress.proxy(server, event, context, {
    apiGatewayEventWhitelist: [],
    apiGatewayContextWhitelist: [],
  });
};
