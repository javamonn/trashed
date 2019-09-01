const path = require('path');

const resolve = require('rollup-plugin-node-resolve');
const commonjs = require('rollup-plugin-commonjs');
const replace = require('rollup-plugin-replace');
const cleaner = require('rollup-plugin-cleaner');
const postcss = require('rollup-plugin-postcss');

const DIST_DIR = path.resolve(__dirname, 'dist');
const BUILD_DIR = path.resolve(DIST_DIR, './build');

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
    resolve(),
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
        ],
        bowser: ['getParser'],
      },
    }),
    replace({
      'process.env.NODE_ENV': JSON.stringify('development'),
    }),
    postcss({
      plugins: [require('tailwindcss')],
    }),
  ],
};
