#!/bin/bash

set -ex

OWN_PATH=`realpath $0`
RPC_SRC_DIR=$(readlink -f "$OWN_PATH/../backend/function/rpc/src")

(cd $RPC_SRC_DIR && npm install)
