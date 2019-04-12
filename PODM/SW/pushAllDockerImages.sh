#!/usr/bin/env bash

set -xe
PROJECT_ROOT=`realpath .`

if [ -z $1 ]; then
    VERSION="1.0-SNAPSHOT"
else
    VERSION=$1
fi

if [ -z $2 ]; then
    export DOCKER_REGISTRY_HOST=localhost:5000
else
    export DOCKER_REGISTRY_HOST=$2
fi

echo "Version: ${VERSION}"
echo "Will push to: ${DOCKER_REGISTRY_HOST}"
./gradlew -Pversion=${VERSION} ${GRADLE_BUILD_OPTS} pushDockerImage

# resource-manager
docker tag resource-manager:${VERSION} ${DOCKER_REGISTRY_HOST}/resource-manager:${VERSION}
docker push ${DOCKER_REGISTRY_HOST}/resource-manager:${VERSION}

# dhcp
cd ${PROJECT_ROOT}/dev-docker/podm-dhcp
./build.sh ${VERSION}
docker tag podm-dhcp:${VERSION} ${DOCKER_REGISTRY_HOST}/podm-dhcp:${VERSION}
docker push ${DOCKER_REGISTRY_HOST}/podm-dhcp:${VERSION}

cd ${PROJECT_ROOT}

set +xe
