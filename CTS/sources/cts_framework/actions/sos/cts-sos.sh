#!/usr/bin/env bash
#
# Copyright (c) 2019 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#
# SOS CTS Report Script
#
# How to use script
#     sh cts-sos.sh
#
# Where I can find results?
#     Reports are generated into <WORK_FOLDER>/sos-report_{date}/ and than compressed into .tar.gz


DATE="$(date +"%d-%m-%Y_%H-%M-%S")"
FOLDER_NAME=sos-report_${DATE}


mkdir -p "${FOLDER_NAME}"
cd "${FOLDER_NAME}"

HOME_DIR=`eval echo ~$SUDO_USER`
USER=$SUDO_USER
DIR_CTS="$HOME_DIR/.cts/"

WHICH_CTS="$(which cts)"
VERSION="$(cts version)"
if [ -z ${WHICH_CTS} ] && [ -z ${VERSION} ]
then
    echo "[FAIL] Can't localize CTS"
else
    echo "[OK]  I found CTS"
    printf "${VERSION}\nLocated: ${WHICH_CTS}\n" > CTS-VERSION.txt
fi


STATUS_LIST="$(cts status list)"
if [ -z ${STATUS_LIST} ]
then
    echo "[FAIL] STATUS_LIST: Can't get data"
else
    echo "[OK]  STATUS_LIST"
    printf "${STATUS_LIST}" > status_list.txt
fi


PIP_LIST="$(pip list --format=columns)"
if [ -z ${PIP_LIST} ]
then
    echo "[FAIL] PIP_LIST: Can't get data"
else
    echo "[OK]  PIP_LIST"
    printf "${PIP_LIST}" > pip_list.txt
fi


PYTHON_DATA="$(python -v output)"
if [ -z ${PYTHON_DATA} ]
then
    echo "[FAIL] PYTHON_DATA: Can't get data"
else
    echo "[OK]  PYTHON_DATA"
    printf "${PYTHON_DATA}" > python_output.txt
fi


CTS_DATA="$(find ${HOME_DIR}/.cts -type f -exec md5sum '{}' +)"
if [ -z ${CTS_DATA} ]
then
    echo "[FAIL] CTS_DATA: Can't get data"
else
    echo "[OK]  CTS_DATA"
    printf "${CTS_DATA}" > cts-test-log.txt
fi


NETWORK_DATA="$(ifconfig -a)"
if [ -z ${NETWORK_DATA} ]
then
    echo "[FAIL] NETWORK_DATA: Can't get data"
else
    printf "${NETWORK_DATA}" > network_data.txt
fi


echo "VAR_LOGS copying..."
VAR_LOGS="$(cp /var/log/cts/cts.log cts.log)"


echo "DATABASE_FILE copying..."
DATABASE_FILE="$(cp ${HOME_DIR}/.cts/db/sqlalchemy_cts sqlalchemy_cts)"


echo "Printing logs from tests"
CTS_TESTS_DUMP="$(cts status dump -a -o text)"

echo "Copying ~/.cts/ directory"
cp -a ${DIR_CTS}/. cts-dir

cd ..
tar -czf ${FOLDER_NAME}.tar.gz ${FOLDER_NAME}

exit
