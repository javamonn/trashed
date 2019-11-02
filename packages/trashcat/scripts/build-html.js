#!/usr/bin/env node

const fs = require('fs')
const path = require('path')

const DIST_DIR = path.resolve(__dirname, '../dist')
const JS_BUILD_DIR = path.resolve(DIST_DIR, './build')
const SRC_DIR = path.resolve(__dirname, '../src')

// Bundle entry should start with this prefix.
const JS_BUNDLE_NAME = 'Index' 
const JS_BUNDLE_DIR = process.env.NODE_ENV === 'production'
  ? JS_BUILD_DIR
  : path.join(JS_BUILD_DIR, 'trashcat/src')
const HTML_TEMPLATE_PATH = path.join(SRC_DIR, 'index.html')

const REPLACE = {
  'BUNDLE_PATH': () => {
    const name = fs.readdirSync(JS_BUNDLE_DIR)
      .find(p => p.startsWith(JS_BUNDLE_NAME))
    const relativePath = 
      path.relative(DIST_DIR, path.join(JS_BUNDLE_DIR, name))
    return "/" + relativePath
  }
}

const template = fs.readFileSync(HTML_TEMPLATE_PATH, { encoding: 'utf-8' })
const output = Object.keys(REPLACE)
  .reduce((template, key) => {
    return template.replace('${' + key + '}', REPLACE[key]())
  }, template)

fs.writeFileSync(path.join(DIST_DIR, 'index.html'), output)
