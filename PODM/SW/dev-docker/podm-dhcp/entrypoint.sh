#!/usr/bin/env sh
set -x
if [ -z "${INTERFACES}" ]; then
    echo "Please provide interface names on which dhcp server should listen on using INTERFACES env variable."
    exit 1
fi

touch /var/lib/dhcp/dhcpd.leases
echo "Starting isc-dhcp-server using interfaces: ${INTERFACES}"
/usr/sbin/dhcpd -f --no-pid -4 -cf /etc/dhcp/dhcpd.conf ${INTERFACES}