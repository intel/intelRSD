#!/usr/bin/env bash

set -xe
PROJECT_ROOT=`realpath .`

if [ -z $1 ]; then
    VERSION="1.0-SNAPSHOT"
else
    VERSION=${1}
fi

./gradlew ${GRADLE_BUILD_OPTS} -Pversion=${VERSION} buildDockerImage

#dhcp
cd ${PROJECT_ROOT}/dev-docker/podm-dhcp
./build.sh ${VERSION}

set +xe
