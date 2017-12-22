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

echo_log "prerm"

cleanup_service() {
    service_name=$1

    if [ -f "/etc/init.d/${service_name}" ]; then
        echo_log "Removing ${service_name} service"
        invoke-rc.d ${service_name} stop
        rm -f /etc/init.d/${service_name}
        update-rc.d ${service_name} remove
        rm -f /var/lib/misc/pod-manager.war.sha512sum
    fi
}

cleanup_wildfly() {
    echo_log "Removing Wildfly Application Server"
    rm -f /etc/default/wildfly
    rm -r -f /opt/pod-manager/wildfly
}

cleanup_podm_services() {
    cleanup_service "wildfly"
    cleanup_service "pod-manager"
}

cleanup_configuration_agent() {
    echo_log "Removing Configuration Agent service"
    if [ -f "/etc/init.d/pod-manager-configuration-agent" ]; then
        invoke-rc.d pod-manager-configuration-agent stop
    fi

    rm -f /etc/init.d/pod-manager-configuration-agent
    update-rc.d pod-manager-configuration-agent remove
}

protect_opt_directory() {
    touch /opt/placeholder
}

cleanup_podm_services
cleanup_wildfly
cleanup_configuration_agent
protect_opt_directory
