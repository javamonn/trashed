#!/bin/bash

set -ex

OWN_PATH=`realpath $0`
FUNCTION_DIR=`realpath "$(dirname $OWN_PATH)/../backend/function/"`

RPC_SRC_DIR="$FUNCTION_DIR/rpc/src"
PRE_SIGNUP_SRC_DIR="$FUNCTION_DIR/trashed452c5745PreSignup/src"
POST_CONFIRMATION_SRC_DIR="$FUNCTION_DIR/trashed452c5745PostConfirmation/src"
DYNAMODB_STREAM_TRIGGER_SRC_DIR="$FUNCTION_DIR/DynamoDBStreamTrigger/src"
MEDIA_CONVERT_JOB_STATE_CHANGE_TRIGGER="$FUNCTION_DIR/MediaConvertJobStateChangeTrigger/src"

# Install depedencies for each of the functions.
(cd $RPC_SRC_DIR && npm install)
(cd $PRE_SIGNUP_SRC_DIR && npm install)
(cd $POST_CONFIRMATION_SRC_DIR && npm install)
(cd $DYNAMODB_STREAM_TRIGGER_SRC_DIR && npm install)
(cd $MEDIA_CONVERT_JOB_STATE_CHANGE_TRIGGER && npm install)

# Sync against the currently deployed GraphQL schema.
(cd $DYNAMODB_STREAM_TRIGGER_SRC_DIR && npm run graphql:sync-schema)
(cd $MEDIA_CONVERT_JOB_STATE_CHANGE_TRIGGER && npm run graphql:sync-schema)
