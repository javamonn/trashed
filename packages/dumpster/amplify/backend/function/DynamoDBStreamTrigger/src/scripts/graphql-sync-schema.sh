#!/bin/bash
set -e

OWN_PATH=`realpath $0`
ROOT_DIR=`realpath "$(dirname $OWN_PATH)/../"`
API_ID=akkeqcns2fc5hn7ad3uu3euymy
AWS_PROFILE=amplify-trashed

aws appsync get-introspection-schema \
  --format JSON \
  --api-id $API_ID \
  --profile $AWS_PROFILE \
  "$ROOT_DIR/graphql_schema.json"
