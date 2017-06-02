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

DHCPD_CONF=/etc/dhcp/dhcpd.conf
INTERFACES_CONF=/etc/network/interfaces
DHCPD_DEFAULT_CONF=/etc/default/isc-dhcp-server

echo_log() {
    echo "[$DPKG_MAINTSCRIPT_PACKAGE]: $1"
}

change_file_permissions() {
    echo_log 'Changing files permissions'

    chmod 755 /etc/init.d/isc-dhcp-server-fix
    chmod 755 /usr/bin/parse_leases.py
}

apply_dhcp_fix() {
    echo_log "Running DHCP server fix..."
    /etc/init.d/isc-dhcp-server-fix start
    echo_log "Configuring isc-dhcp-server-fix service"
    update-rc.d isc-dhcp-server-fix defaults
}

add_listening_interfaces_for_dhcpd() {
    echo_log "Updating /etc/default/isc-dhcp-server file"
    actual_interfaces="$(grep 'INTERFACES' $DHCPD_DEFAULT_CONF | awk -F\" '{print $(NF-1)}')"
    updated_interfaces="$(echo "$actual_interfaces" | sed -e '/eth0.4091/ ! s/$/ eth0.4091/g' -e '/eth0.4093/ ! s/$/ eth0.4093/g' -e '/eth0.4094/ ! s/$/ eth0.4094/g' -e '/eth0.4088/ ! s/$/ eth0.4088/g')"
    sed -i "s/INTERFACES=.*/INTERFACES=\"$updated_interfaces\"/g" $DHCPD_DEFAULT_CONF
}

configure_dhcp() {
    INCLUDE='include "/etc/dhcp/dhcpd-pod-manager.inc";'
    INCLUDE_LEGACY='include "/etc/dhcp/dhcpd-rsa.inc";'

    # removing include for legacy include file
    sed -i 's,'"$INCLUDE_LEGACY"',,g' "$DHCPD_CONF"

    echo_log "Changing dhcp configuration to pod-manager specific"
    grep "$INCLUDE" $DHCPD_CONF > /dev/null || echo "$INCLUDE" >> $DHCPD_CONF

    add_listening_interfaces_for_dhcpd

    echo_log "Restarting isc-dhcp-server"
    service isc-dhcp-server restart || :
}

disable_apparmor() {
    if [ ! -e /etc/init.d/apparmor ]; then
        return
    fi

    echo_log "Killing apparmor service"
    /etc/init.d/apparmor stop 2> /dev/null || :
    echo_log "Disabling apparmor service"
    update-rc.d -f apparmor remove || :
}

configure_ethernet_interfaces() {
    INTERFACE_SOURCE='source /etc/network/interfaces.d/pod-manager-network-configuration.conf'

    echo_log "Adding POD Manager specific network configuration to current interfaces configuration"
    grep "$INTERFACE_SOURCE" $INTERFACES_CONF > /dev/null || echo "$INTERFACE_SOURCE" >> $INTERFACES_CONF

    # FIXME: Ubuntu 14.04 disabled restarting networking
    echo_log "Restarting networking service"
    service networking restart || :
}

configure_ntp() {
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

enable_vlans_module() {
    MODULES_FILE="/etc/modules"
    grep -sq "^8021q" $MODULES_FILE || { echo "8021q" >> $MODULES_FILE; modprobe 8021q || : ; }
}

echo_log "postinst: beginning $1 action"

# DO NOT CHANGE ANYTHING UNDER THIS LINE
# This order is a fix for conflict between isc-dhcp-server and networking service in Ubuntu

change_file_permissions
disable_apparmor
enable_vlans_module
configure_ethernet_interfaces
configure_ntp
configure_tftpd
apply_dhcp_fix
configure_dhcp

echo_log "postinst: done"
