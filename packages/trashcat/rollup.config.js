const path = require('path');

const resolve = require('rollup-plugin-node-resolve');
const commonjs = require('rollup-plugin-commonjs');
const replace = require('rollup-plugin-replace');
const cleaner = require('rollup-plugin-cleaner');
const postcss = require('rollup-plugin-postcss');
const json = require('rollup-plugin-json');
const polyfill = require('rollup-plugin-polyfill');
const builtins = require('rollup-plugin-node-builtins');

const DIST_DIR = path.resolve(__dirname, 'dist');
const BUILD_DIR = path.resolve(DIST_DIR, './build');

const REQUIRE_ALIAS = [
  {
    from: 'aws-sdk/clients/s3',
    to: 'aws-sdk/dist/aws-sdk.js',
    prop: '.S3',
  },
  {
    from: 'aws-sdk/global',
    to: 'aws-sdk/dist/aws-sdk.js',
  },
  {
    from: 'aws-sdk/browser',
    to: 'aws-sdk/dist/aws-sdk',
  },
].reduce(
  (acc, {from, to, prop}) => ({
    ...acc,
    [`require("${from}")`]: `require("${to}")${prop ? prop : ''}`,
    [`require('${from}')`]: `require("${to}")${prop ? prop : ''}`,
  }),
  {},
);

module.exports = {
  input: './src/Index.bs.js',
  output: {
    dir: BUILD_DIR,
    entryFileNames: '[name].mjs',
    format: 'esm',
  },
  preserveModules: true,
  plugins: [
    cleaner({targets: [BUILD_DIR]}),
    json(),
    builtins(),
    replace({
      delimiters: ['', ''],
      values: {
        'process.env.NODE_ENV': JSON.stringify('development'),
        ...REQUIRE_ALIAS,
      },
    }),
    polyfill(['./polyfill.js']),
    resolve({browser: true}),
    commonjs({
      include: 'node_modules/**',
      namedExports: {
        'react-dom': ['render', 'hydrate'],
        react: [
          'createElement',
          'useEffect',
          'useRef',
          'forwardRef',
          'useReducer',
          'useState',
          'createContext',
          'useContext',
          'Component',
        ],
        bowser: ['getParser'],
        ogv: ['OGVPlayer', 'OGVLoader'],
        'prop-types': [
          'object',
          'func',
          'node',
          'string',
          'bool',
          'number',
          'oneOfType',
          'arrayOf',
          'any',
        ],
        'js-cookie': ['get', 'set', 'remove'],
        '@aws-amplify/auth': ['currentSession'],
        '@aws-amplify/core': ['configure'],
      },
    }),
    postcss({
      plugins: [require('tailwindcss')],
    }),
  ],
};
