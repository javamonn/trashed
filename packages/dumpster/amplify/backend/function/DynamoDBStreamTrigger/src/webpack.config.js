const path = require('path')
const webpack = require('webpack')

module.exports = (env) => ({
  target: 'node',
  mode: process.env.NODE_ENV || 'development',
  entry: [path.resolve(__dirname, './app/Index.bs.js')],
  output: {
    filename: 'bundle.js',
    libraryTarget: 'umd'
  },
  plugins: [
      new webpack.EnvironmentPlugin(
        process.env.NODE_ENV === 'production' 
          ? [
            'MEDIACONVERT_ENDPOINT',
            'AUTH_TRASHED452C5745_USERPOOLWEBCLIENTID'
          ]
          : [
            'MEDIACONVERT_ENDPOINT',
            'MEDIACONVERT_IAM_ROLE_ARN',
            'AUTH_TRASHED452C5745_USERPOOLWEBCLIENTID',
            'AUTH_TRASHED452C5745_USERPOOLID',
            'API_TRASHED_GRAPHQLAPIENDPOINTOUTPUT',
            'STORAGE_TRASHCAT_BUCKETNAME',
            'REGION'
          ]
      )
  ]
})
