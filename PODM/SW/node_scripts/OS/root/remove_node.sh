#!/bin/bash

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

removing_failed_read_only_filesystem ()
{
 echo "Removing failed, the filesystem is in read only mode."
 exit 1
}

if [ "$(id -u)" != "0" ]; then
   echo "This script must be run as root."
   exit 1
fi

if [ ! -f /root/created ]
then
  echo "The setup script was not run. Run the setup script first."
  exit 1
fi

rm /root/created || removing_failed_read_only_filesystem

#nova-manage service disable $1 nova-compute
ovs-vsctl del-br br-int
service nova-compute stop
service openvswitch-switch stop
service neutron-plugin-openvswitch-agent stop

mysql -u nova -prsa --host controller << EOF
use nova;
delete from nova.compute_nodes where service_id=(select id from services where host="$HOSTNAME");
delete from nova.services where host="$HOSTNAME";
EOF

cp /root/configs/restore/nova.conf /etc/nova/nova.conf
cp /root/configs/restore/neutron.conf /etc/neutron/neutron.conf
cp /root/configs/restore/ml2_conf.ini /etc/neutron/plugins/ml2/ml2_conf.ini

IXGBE=`grep "eth1.100:" /proc/net/dev`
if  [ -n "$IXGBE" ] ; then
    INTERFACE="eth1"
  else
    INTERFACE="eth0"
fi

ifconfig $INTERFACE.100 down
ifconfig $INTERFACE.200 down
vconfig rem $INTERFACE.100
vconfig rem $INTERFACE.200
cp /root/configs/restore/interfaces /etc/network/interfaces

sed "/^server /d" -i /etc/ntp.conf
sed "s/#server 127.127.1.0/server 127.127.1.0/g" -i /etc/ntp.conf
sed "s/#fudge 127.127.1.0 stratum 10/fudge 127.127.1.0 stratum 10/g#" -i /etc/ntp.conf
service ntp restart

HOSTNAME="ubuntu1404"
hostname $HOSTNAME
echo $HOSTNAME > /etc/hostname
cp /root/configs/restore/hosts /etc/hosts

exit 0

