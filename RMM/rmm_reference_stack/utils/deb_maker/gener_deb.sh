#!/bin/bash
CUR_PATH=$(cd "$(dirname "$0")"; pwd)
BUILD_PATH=$CUR_PATH/../../build/

cp -r $CUR_PATH/install $BUILD_PATH

$BUILD_PATH/install/multi-deb/sh/prepare.sh
$BUILD_PATH/install/multi-deb/sh/generate_deb.sh
$BUILD_PATH/install/allinone-deb/sh/prepare.sh
$BUILD_PATH/install/allinone-deb/sh/generate_deb.sh