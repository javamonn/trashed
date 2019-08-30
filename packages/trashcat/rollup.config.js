const path = require('path');
const resolve = require('rollup-plugin-node-resolve');
const commonjs = require('rollup-plugin-commonjs');
const replace = require('rollup-plugin-replace');

module.exports = {
  input: './src/Index.js',
  output: {
    file: path.resolve(__dirname, 'dist') + '/build/bundle.js',
    format: 'es',
  },
  plugins: [
    resolve({
      browser: true,
    }),
    commonjs({
      include: 'node_modules/**',
      namedExports: {
        'react-dom': ['render', 'hydrate'],
        react: ['createElement', 'useEffect'],
      },
    }),
    replace({
      'process.env.NODE_ENV': JSON.stringify('development'),
    }),
  ],
};
