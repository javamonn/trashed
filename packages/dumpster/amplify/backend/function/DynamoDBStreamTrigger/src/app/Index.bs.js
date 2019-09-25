// Generated by BUCKLESCRIPT VERSION 5.0.6, PLEASE EDIT WITH CARE
'use strict';

var Curry = require("bs-platform/lib/js/curry.js");
var Belt_Array = require("bs-platform/lib/js/belt_Array.js");
var DynamoDBStreamRecord$DynamoDBStreamTrigger = require("./DynamoDBStreamRecord.bs.js");

var Lambda = /* module */[];

function handleMediaConvertSubmit(r) {
  return Promise.resolve(/* () */0);
}

function handleMediaConvertComplete(r) {
  return Promise.resolve(/* () */0);
}

function handler($$event, _context, cb) {
  Promise.all(Belt_Array.map($$event.Records, (function (r) {
                  var match = r.eventName;
                  DynamoDBStreamRecord$DynamoDBStreamTrigger.tableGet(r);
                  switch (match) {
                    case "INSERT" : 
                    case "UPDATE" : 
                        return Promise.resolve(/* () */0);
                    default:
                      return Promise.resolve(/* () */0);
                  }
                }))).then((function (param) {
            Curry._2(cb, null, $$event);
            return Promise.resolve(/* () */0);
          })).catch((function (ex) {
          console.log(ex);
          Curry._2(cb, ex, $$event);
          return Promise.resolve(/* () */0);
        }));
  return /* () */0;
}

exports.Lambda = Lambda;
exports.handleMediaConvertSubmit = handleMediaConvertSubmit;
exports.handleMediaConvertComplete = handleMediaConvertComplete;
exports.handler = handler;
/* No side effect */
