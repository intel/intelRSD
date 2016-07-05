# Copyright (c) 2015 Intel Corporation
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

echo_log "prerm"

cleanup_wildfly() {
    echo_log "Removing Wildfly service"
    if [ -f "/etc/init.d/wildfly" ]; then
        service wildfly stop
    fi

    rm -f /etc/init.d/wildfly
    rm -f /etc/default/wildfly
    update-rc.d wildfly remove
    rm -r -f /opt/pod-manager/wildfly
}

cleanup_orientdb() {
    echo_log "Removing Orient service"
    if [ -f "/etc/init.d/orientdb" ]; then
        service orientdb stop
    fi

    rm -f /etc/init.d/orientdb
    update-rc.d orientdb remove
    rm -r -f /opt/pod-manager/orientdb
}

cleanup_configuration_agent() {
    echo_log "Removing Configuration Agent service"
    if [ -f "/etc/init.d/pod-manager-configuration-agent" ]; then
        service pod-manager-configuration-agent stop
    fi

    rm -f /etc/init.d/pod-manager-configuration-agent
    update-rc.d pod-manager-configuration-agent remove
}

protect_opt_directory() {
    touch /opt/placeholder
}

cleanup_wildfly
cleanup_orientdb
cleanup_configuration_agent
protect_opt_directory