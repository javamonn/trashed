// Generated by BUCKLESCRIPT VERSION 5.0.6, PLEASE EDIT WITH CARE
'use strict';

var Belt_Array = require("bs-platform/lib/js/belt_Array.js");
var Belt_Option = require("bs-platform/lib/js/belt_Option.js");
var Js_mapperRt = require("bs-platform/lib/js/js_mapperRt.js");

var Field = /* module */[];

var VideoRecord = /* module */[];

var jsMapperConstantArray = /* array */[/* tuple */[
    -580716109,
    "Video-akkeqcns2fc5hn7ad3uu3euymy-production"
  ]];

function tableNameToJs(param) {
  return Js_mapperRt.binarySearch(1, param, jsMapperConstantArray);
}

function tableNameFromJs(param) {
  return Js_mapperRt.revSearch(1, jsMapperConstantArray, param);
}

function tableGet(inst) {
  return Belt_Option.flatMap(Belt_Array.get(inst.eventSourceARN.split("/"), 1), tableNameFromJs);
}

exports.Field = Field;
exports.VideoRecord = VideoRecord;
exports.tableNameToJs = tableNameToJs;
exports.tableNameFromJs = tableNameFromJs;
exports.tableGet = tableGet;
/* No side effect */
