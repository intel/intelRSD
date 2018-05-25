# Copyright (c) 2015-2018 Intel Corporation
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

echo_log() {
    echo "[${DPKG_MAINTSCRIPT_PACKAGE}]: ${1}"
}
echo_log "preinst: beginning ${1} action"

USER_NAME="podm"
GROUP_NAME=$USER_NAME
PODM_APPLICATION_SERVER_HOME="/opt/pod-manager/wildfly"

getent group $GROUP_NAME > /dev/null || groupadd -r $GROUP_NAME
getent passwd $USER_NAME > /dev/null || useradd -r -g $GROUP_NAME -d $PODM_APPLICATION_SERVER_HOME -s /usr/sbin/nologin $USER_NAME

echo_log "preinst: done"
exit 0
