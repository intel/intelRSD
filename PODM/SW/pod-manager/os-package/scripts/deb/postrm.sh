# Copyright (c) 2015-2017 Intel Corporation
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

cleanup_psql_data() {
    echo_log "Removing PostgreSQL data"
    #kill remaining connections
    service postgresql restart
    sudo -u postgres dropdb --if-exists podm
    sudo -u postgres dropuser --if-exists podm
}

case "$1" in
    purge)
        cleanup_psql_data
    ;;
esac
