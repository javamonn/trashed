#!/bin/bash

set -ex

OWN_PATH=`realpath $0`
FUNCTION_DIR=`readlink -f "$(dirname $OWN_PATH)/../backend/function/"`

DYNAMODB_STREAM_TRIGGER_SRC_DIR="$FUNCTION_DIR/DynamoDBStreamTrigger/src"
MEDIA_CONVERT_JOB_STATE_CHANGE_TRIGGER_SRC_DIR="$FUNCTION_DIR/MediaConvertJobStateChangeTrigger/src"
RPC_SRC_DIR="$FUNCTION_DIR/rpc/src"

# Remove node_modules to avoid bloating deploy artifact.
(cd $DYNAMODB_STREAM_TRIGGER_SRC_DIR && rm -rf node_modules)
(cd $MEDIA_CONVERT_JOB_STATE_CHANGE_TRIGGER_SRC_DIR && rm -rf node_modules)
(cd $RPC_SRC_DIR && rm -rf node_modules)
