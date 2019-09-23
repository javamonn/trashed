const path = require('path');
const webpack = require('webpack');

module.exports = {
  target: 'node',
  mode: process.env.NODE_ENV || 'development',
  entry: [path.resolve(__dirname, './app/Index.bs.js')],
  output: {
    filename: 'bundle.js',
    libraryTarget: 'umd',
  },
  plugins: [
    new webpack.EnvironmentPlugin([
      'NODE_ENV',
      'AWS_ACCESS_KEY_ID',
      'AWS_SECRET_ACCESS_KEY',
    ]),
  ],
};
