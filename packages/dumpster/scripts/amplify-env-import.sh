#!/bin/bash

set -e
IFS='|'
OWN_PATH=`realpath $0`
AWSCLOUDFORMATIONCONFIG="{\
  \"Region\": \"us-east-1\",\
  \"DeploymentBucketName\": \"trashed-production-20190824131356-deployment\",\
  \"UnauthRoleName\": \"trashed-production-20190824131356-unauthRole\",\
  \"StackName\": \"trashed-production-20190824131356\",\
  \"StackId\": \"arn:aws:cloudformation:us-east-1:046525304497:stack/trashed-production-20190824131356/8e7ca8d0-c692-11e9-8351-0a16600979f0\",\
  \"AuthRoleName\": \"trashed-production-20190824131356-authRole\",\
  \"UnauthRoleArn\": \"arn:aws:iam::046525304497:role/trashed-production-20190824131356-unauthRole\",\
  \"AuthRoleArn\": \"arn:aws:iam::046525304497:role/trashed-production-20190824131356-authRole\"\
}"
PROVIDER_CONFIG="{\
  \"awscloudformation\":$AWSCLOUDFORMATIONCONFIG\
}"
AWS_CONFIG="{\
  \"configLevel\":\"project\",\
  \"useProfile\":true,\
  \"profileName\":\"amplify-trashed\"\
}"

amplify env import \
  --name production \
  --config $PROVIDER_CONFIG \
  --awsInfo $AWS_CONFIG \
  --yes
