#!/usr/bin/env bash

if [ -z ${1} ]; then
    VERSION="1.0-SNAPSHOT"
else
    VERSION=${1}
fi
docker build ${DOCKER_BUILD_OPTS} --tag podm-dhcp:${VERSION} .
