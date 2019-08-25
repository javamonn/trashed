#!/bin/bash

set -ex

OWN_PATH=`realpath $0`
FUNCTION_DIR=`readlink -f "$(dirname $OWN_PATH)/../backend/function/"`

RPC_SRC_DIR="$FUNCTION_DIR/rpc/src"
PRESIGNUP_SRC_DIR="$FUNCTION_DIR/trashed452c5745PreSignup/src"

# Build each of the functions.
(cd $RPC_SRC_DIR && npm run build)
(cd $PRESIGNUP_SRC_DIR && npm run build)
