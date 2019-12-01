const path = require('path');
const webpack = require('webpack');
const {CleanWebpackPlugin} = require('clean-webpack-plugin');
const {BundleAnalyzerPlugin} = require('webpack-bundle-analyzer');

const DIST_DIR = path.resolve(__dirname, 'dist');
const BUILD_DIR = path.resolve(DIST_DIR, './js');

module.exports = {
  mode: process.env.NODE_ENV || 'development',
  entry: './src/Index.bs.js',
  output: {
    path: BUILD_DIR,
    filename:
      process.env.NODE_ENV === 'production' ? 'bundle-[hash].js' : 'bundle.js',
  },
  resolve: {
    alias: {
      'apollo-client': require.resolve('apollo-client'),
      'apollo-cache-inmemory': require.resolve('apollo-cache-inmemory'),
    },
  },
  module: {
    rules: [
      {
        test: /\.js$/,
        type: 'javascript/auto',
        exclude: /node_modules/,
      },
      {
        test: /\.pcss$/,
        use: [
          'style-loader',
          {loader: 'css-loader', options: {importLoaders: 1}},
          {
            loader: 'postcss-loader',
            options: {
              plugins: [
                require('tailwindcss'),
                process.env.NODE_ENV === 'production'
                  ? require('@fullhuman/postcss-purgecss')({
                      content: ['./src/**/*.bs.js'],
                    })
                  : null,
              ].filter(p => Boolean(p)),
            },
          },
        ],
      },
      {
        test: /\.svg$/,
        loader: 'file-loader',
        options: {
          outputPath: 'static',
          esModule: false,
          publicPath: '/js/static',
          name:
            process.env.NODE_ENV === 'production'
              ? '[contenthash].[ext]'
              : '[name].[ext]',
        },
      },
    ],
  },
  plugins: [
    process.env.ANALYZE_BUNDLE ? new BundleAnalyzerPlugin() : null,
    new CleanWebpackPlugin(),
    new webpack.DefinePlugin({
      'process.env.NODE_ENV': JSON.stringify(
        process.env.NODE_ENV || 'development',
      ),
      'process.env.CLOUDFRONT_DISTRIBUTION_ORIGIN': JSON.stringify(
        'trashcat-cdn.trashed.today',
      ),
      'process.env.RPC_ORIGIN': JSON.stringify('trashcat-rpc.trashed.today'),
    }),
  ].filter(p => Boolean(p)),
};
