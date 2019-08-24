const path = require('path')
const webpack = require('webpack')

module.exports = {
  target: 'node',
  mode: process.env.NODE_ENV || 'development',
  entry: [path.resolve(__dirname, './index.js')],
  output: {
    path: path.resolve(__dirname, 'dist'),
    filename: 'bundle.js'
  },
  plugins: [
    new webpack.EnvironmentPlugin([
      'NODE_ENV',
      'TWILIO_PHONE_NUMBER',
      'TWILIO_SID',
      'TWILIO_AUTH_TOKEN'
    ])
  ]
}
