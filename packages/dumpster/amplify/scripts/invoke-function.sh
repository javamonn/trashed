#!/usr/bin/expect

set function [lindex $argv 0]
set event [lindex $argv 1]

spawn amplify function invoke $function

expect "? Provide the name of the script file that contains your handler function:"
send "local-invoke.js\n"

expect "? Provide the name of the handler function to invoke:"
send "handler\n"

expect "? Provide the relative path to the event:"
send "./invoke-events/$event.json\n"

interact
