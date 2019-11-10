#!/bin/bash
set -e

OWN_PATH=`realpath $0`
ROOT_DIR=`readlink -f "$(dirname $OWN_PATH)/../"`
API_ID=akkeqcns2fc5hn7ad3uu3euymy

aws appsync get-introspection-schema \
  --format JSON \
  --api-id $API_ID \
  "$ROOT_DIR/graphql_schema.json"
