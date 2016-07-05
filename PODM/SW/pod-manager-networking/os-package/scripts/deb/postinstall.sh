# Copyright (c) 2015-2016 Intel Corporation
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

DHCPD_CONF=/etc/dhcp/dhcpd.conf

echo_log() {
    echo "[$DPKG_MAINTSCRIPT_PACKAGE]: $1"
}

change_permissions() {
    echo_log 'Changing files permissions'

    chmod 755 /etc/init.d/isc-dhcp-server-fix
    chmod 755 /usr/bin/parse.leases.py
}

configure_dhcp_service() {
    echo_log "Configuring isc-dhcp-server-fix service"
    update-rc.d isc-dhcp-server-fix defaults 19
}

apply_dhcp_fix() {
    echo_log "Applying DHCP server fix..."
    setfacl -dm u:dhcpd:rwx /var/lib/dhcp
    setfacl -m u:dhcpd:rwx /var/lib/dhcp
}

append_dhcp_config() {
    echo_log "Changing dhcp configuration to pod-manager specific"
    echo "$INCLUDE" >> $DHCPD_CONF
    echo_log "Restarting isc-dhcp-server"
    service isc-dhcp-server restart || :
}

handle_dhcp_configuration() {
    INCLUDE='include "/etc/dhcp/dhcpd-pod-manager.inc";'
    INCLUDE_LEGACY='include "/etc/dhcp/dhcpd-rsa.inc";'

    # removing include for legacy include file
    sed -i 's,'"$INCLUDE_LEGACY"',,g' "$DHCPD_CONF"

    # TODO: os-package uses "#!/bin/sh -e" as shell - checking exit status with #? is not possible
    grep "$INCLUDE" $DHCPD_CONF > /dev/null || append_dhcp_config
}

handle_apparmor() {
    if [ ! -e /etc/init.d/apparmor ]; then
        return
    fi

    echo_log "Killing apparmor service"
    /etc/init.d/apparmor stop 2> /dev/null || :
    echo_log "Disabling apparmor service"
    update-rc.d -f apparmor remove || :
}


handle_etc_network_interfaces() {
    echo_log "Overriding /etc/network/interfaces to pod-manager specific configuration"
    cp -f /etc/pod-manager/opt/network/interfaces /etc/network/interfaces

    # FIXME: Ubuntu 14.04 disabled restarting networking
    echo_log "Restarting networking service"
    service networking restart || :
}

handle_etc_ntp_conf() {
    echo_log "Overriding /etc/ntp.conf to pod-manager specific configuration"
    cp -f /etc/pod-manager/opt/ntp.conf /etc/ntp.conf
    echo_log "Restarting ntp service"
    service ntp restart || :
}

configure_tftpd() {
    echo_log "Configuring TFTPd"
    grep -v TFTP_DIRECTORY /etc/default/tftpd-hpa > /tmp/tftpd-hpa
    echo 'TFTP_DIRECTORY="/srv/tftp"' >> /tmp/tftpd-hpa
    mv -f /tmp/tftpd-hpa /etc/default/tftpd-hpa
}

handle_tagged_vlans() {
    MODULES_FILE="/etc/modules"
    grep -sq "^8021q" $MODULES_FILE || { echo "8021q" >> $MODULES_FILE; modprobe 8021q || : ; }
}

echo_log "postinst: beginning $1 action"

change_permissions
handle_apparmor
handle_dhcp_configuration
configure_dhcp_service
apply_dhcp_fix
handle_tagged_vlans
# todo: move this to separate script?
handle_etc_network_interfaces
handle_etc_ntp_conf
#
configure_tftpd

echo_log "postinst: done"
