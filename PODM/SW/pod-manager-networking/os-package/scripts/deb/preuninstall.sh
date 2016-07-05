# Copyright (c) 2016 Intel Corporation
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

DHCPD_CONF=/etc/dhcp/dhcpd.conf
INCLUDE='include "/etc/dhcp/dhcpd-pod-manager.inc";'
TMP_DHCPD_CONF=/tmp/dhcpd.conf

# removing include in dhcpd conf file
grep -v "$INCLUDE" "$DHCPD_CONF" > "$TMP_DHCPD_CONF"
mv "$TMP_DHCPD_CONF" "$DHCPD_CONF"

sudo service isc-dhcp-server-fix stop
rm -f /etc/init.d/isc-dhcp-server-fix
update-rc.d isc-dhcp-server-fix remove

echo_log "postrm"
