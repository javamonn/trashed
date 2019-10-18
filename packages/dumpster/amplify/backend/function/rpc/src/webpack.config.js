const path = require('path');
const webpack = require('webpack');

module.exports = {
  target: 'node',
  mode: process.env.NODE_ENV || 'development',
  entry: [path.resolve(__dirname, './index.js')],
  output: {
    filename: 'bundle.js',
    libraryTarget: 'umd',
  },
  plugins: [
    new webpack.EnvironmentPlugin(
      process.env.NODE_ENV === 'production'
        ? ['NODE_ENV', 'CLOUDFRONT_KEY_PAIR_ID', 'CLOUDFRONT_PRIVATE_KEY']
        : [
            'NODE_ENV',
            'CLOUDFRONT_KEY_PAIR_ID',
            'CLOUDFRONT_PRIVATE_KEY',
            'CLOUDFRONT_DISTRIBUTION_ORIGIN',
          ],
    ),
  ],
};
