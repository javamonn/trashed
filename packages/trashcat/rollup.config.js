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

const globalShim = (r, p) => `(require("${r}"), global.${p})`;
const defaultShim = ({to, namedExports}) =>
  `({ 
  default: require("${to}"),
   ${Object.keys(namedExports || {}).reduce((acc, k) => {
     acc += `${k}: require("${namedExports[k].path}")${
       namedExports[k].prop ? `.${namedExports[k].prop}` : ''
     },`;
     return acc;
   }, '')}
})`.replace('\n', '');

const buildAlias = ({to, prop, global_, default_, namedExports}) =>
  global_
    ? globalShim(to, global_) + (prop ? prop : '')
    : default_
    ? defaultShim({to, namedExports})
    : `require("${to}")${prop ? prop : ''}`;

/**
 * aws-sdk has numerous nasty circular dependencies, so we avoid the
 * problem entirely by using the full browser build.
 *
 * Other packages do not play nicely with rollup's default export semantics,
 * so we wrap the require statement.
 */

const REQUIRE_ALIAS = [
  {
    from: 'aws-sdk/clients/s3',
    to: 'aws-sdk/dist/aws-sdk.js',
    prop: '.S3',
    global_: 'AWS',
  },
  {
    from: 'aws-sdk/global',
    to: 'aws-sdk/dist/aws-sdk.js',
    global_: 'AWS',
  },
  {
    from: 'aws-sdk/browser',
    to: 'aws-sdk/dist/aws-sdk.js',
    global_: 'AWS',
  },
  {
    from: 'apollo-client',
    to: 'apollo-client',
    default_: true,
    namedExports: {
      ApolloError: {
        path: 'apollo-client/errors/ApolloError',
        prop: 'ApolloError',
      },
    },
  },
  {
    from: 'redux-thunk',
    to: 'redux-thunk',
    default_: true,
  },
].reduce(
  (acc, {from, ...alias}) => ({
    ...acc,
    [`require("${from}")`]: buildAlias(alias),
    [`require('${from}')`]: buildAlias(alias),
  }),
  {},
);

console.log(REQUIRE_ALIAS);

/**
 * Some bucklescript dependencies bind directly to modules (`import * as Foo,
 * Foo()`), which rollup does not like. Rewrite import the default.
 */
const IMPORT_ALIAS = {
  'import * as GraphqlTag from "graphql-tag"':
    'import GraphqlTag from "graphql-tag"',
};

const config = {
  input: './src/Index.bs.js',
  output: {
    dir: BUILD_DIR,
    entryFileNames: '[name].mjs',
    format: 'esm',
    sourcemap: false,
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
        ...IMPORT_ALIAS,
        ...REQUIRE_ALIAS,
      },
    }),
    polyfill(['./polyfill.js']),
    resolve({
      browser: true,
      dedupe: [
        'apollo-cache-inmemory',
        'apollo-client',
        '@aws-amplify/api',
        '@aws-amplify/storage',
        '@aws-amplfiy/auth',
        '@aws-amplify/cache',
        '@aws-amplify/core',
        '@aws-amplify/pubsub',
      ],
    }),
    commonjs({
      ignoreGlobal: true,
      sourcemap: false,
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
          'useMemo',
          'Component',
        ],
        bowser: ['getParser'],
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

module.exports = config;
