#!/usr/bin/env bash
set -xe

CHART_VERSION="0.99"
APP_VERSION="1.0-SNAPSHOT"

PODM_VALUES_PATH="helm/pod-manager/values.yaml"
PODM_VALUES_TEMPLATE_PATH="helm/pod-manager/values-template.yaml"

function print_help {
    echo "Usage": createHelmChart [options] >&2 ;
    echo "OPTIONS:"
    echo -e "\t-c chartVersion, default value: $CHART_VERSION"
    echo -e "\t-a appVersion, default value: $APP_VERSION"
}

while getopts c:a:h option
do
case "${option}"
in
c) CHART_VERSION=${OPTARG};;
a) APP_VERSION=${OPTARG};;
h) print_help; exit 2;
esac
done


cp ${PODM_VALUES_TEMPLATE_PATH} ${PODM_VALUES_PATH}
sed --in-place "s%{{ APP_VERSION }}%${APP_VERSION}%g" ${PODM_VALUES_PATH}

helm package \
    --dependency-update \
    --version ${CHART_VERSION} \
    --save=false \
    --app-version ${APP_VERSION} \
    helm/pod-manager

printf "" > ${PODM_VALUES_PATH}

set +xe