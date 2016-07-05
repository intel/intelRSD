#!/bin/bash
CUR_PATH=$(cd "$(dirname "$0")"; pwd)
PROJ_PATH=$CUR_PATH/../../../..

ver=$(cat $PROJ_PATH/src/VERSION)
sed "/Version:/{s/.*$/Version: ${ver}/}" $1/DEBIAN/control > ./control.tmp
mv ./control.tmp $1/DEBIAN/control
