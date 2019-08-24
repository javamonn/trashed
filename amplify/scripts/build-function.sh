#!/bin/bash

set -ex

OWN_PATH=`realpath $0`
RPC_SRC_DIR=$(readlink -f $(dirname "$OWN_PATH")/../backend/function/rpc/src)

# Build the RPC app.
(cd $RPC_SRC_DIR && npm run build)
