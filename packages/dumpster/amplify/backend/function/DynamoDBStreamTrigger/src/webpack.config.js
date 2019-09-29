const path = require('path')
const webpack = require('webpack')

module.exports = {
  target: 'node',
  mode: process.env.NODE_ENV || 'development',
  entry: [path.resolve(__dirname, './app/Index.bs.js')],
  output: {
    filename: 'bundle.js',
    libraryTarget: 'umd'
  },
  plugins: []
}
