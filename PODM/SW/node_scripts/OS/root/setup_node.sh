#!/bin/bash

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

setup_failed_no_connection ()
{
  echo "Setup failed, no connection to Cloud Controller."
  rm /root/created
  ifconfig $INTERFACE.100 down
  ifconfig $INTERFACE.200 down
  vconfig rem $INTERFACE.100
  vconfig rem $INTERFACE.200
  cp /root/configs/restore/interfaces /etc/network/interfaces  
  service ntp start
  exit 1
}

setup_failed_read_only_filesystem ()
{
 echo "Setup failed, the filesystem is in read only mode."
 exit 1
}

if [ "$(id -u)" != "0" ]; then
   echo "This script must be run as root."
   exit 1
fi

if [ -z "$1" ]; then
  echo "Supply the IP of Controller Node as first argument."
  exit 1
fi

if [ -f /root/created ]
then
  echo "The setup script has been already run. Run the removing script, then run the setup script again."
  exit 1
fi

touch /root/created || setup_failed_read_only_filesystem

<<IGB_ONLY
#Checking whether IXGBE network card is present
IXGBE=`grep "eth1:" /proc/net/dev`
if  [ -n "$IXGBE" ] ; then
    INTERFACE="eth1" # IXGBE set in /etc/udev/rules.d/70-persistent-net.rules to always be eth1 inteface 
    #dhclient eth1 # commented out since Xeon uses this interface for iSCSI connection
  else
    INTERFACE="eth0"
fi
IGB_ONLY
INTERFACE="eth0"
dhclient $INTERFACE

#VLANs setup
IP_ETH0=`ifconfig $INTERFACE | grep -Eo 'inet (addr:)?([0-9]*\.){3}[0-9]*' | grep -Eo '([0-9]*\.){3}[0-9]*'`
IP_ETH0_LAST_DIGITS=`echo $IP_ETH0 | cut -d '.' -f4`

vconfig add $INTERFACE 100
vconfig add $INTERFACE 200

IP_ETH0_100=100.100.100.$IP_ETH0_LAST_DIGITS
IP_ETH0_200=200.200.200.$IP_ETH0_LAST_DIGITS

ifconfig $INTERFACE.100 $IP_ETH0_100 netmask 255.255.255.0 up 
ifconfig $INTERFACE.200 $IP_ETH0_200 netmask 255.255.255.0 up 

cat <<EOF >> /etc/network/interfaces

auto $INTERFACE.100
iface $INTERFACE.100 inet static
    address $IP_ETH0_100
    netmask 255.255.255.0
    vlan-raw-device $INTERFACE

auto $INTERFACE.200
iface $INTERFACE.200 inet static
    address $IP_ETH0_200
    netmask 255.255.255.0
    vlan-raw-device $INTERFACE	
EOF

#Synchronizing time
service ntp stop
ntpdate $1 || setup_failed_no_connection
sed "s/server 127.127.1.0/#server 127.127.1.0/g" -i /etc/ntp.conf
sed "s/fudge 127.127.1.0 stratum 10/#fudge 127.127.1.0 stratum 10/g#" -i /etc/ntp.conf
sed "/#fudge 127.127.1.0 stratum 10/aserver $1" -i  /etc/ntp.conf
service ntp start

#Hostname setup
if [ -z "$2" ]; then
HOSTNAME="compute-node-$IP_ETH0_LAST_DIGITS"
else
HOSTNAME=$2	
fi

hostname $HOSTNAME
echo "$HOSTNAME" > /etc/hostname
sed -i "1i$IP_ETH0    $HOSTNAME" /etc/hosts

#Compute Node setup
sed -i "1i$1	controller" /etc/hosts

sed "s/local_ip = .*/local_ip = $IP_ETH0_200/g" -i /root/configs/ml2_conf.ini
sed "s/my_ip = .*/my_ip = $IP_ETH0_100/g" -i /root/configs/nova.conf
sed "s/vncserver_proxyclient_address = .*/vncserver_proxyclient_address = $IP_ETH0_100/g" -i /root/configs/nova.conf
sed "s/novncproxy_base_url = .*/novncproxy_base_url = http:\/\/$1:6080\/vnc_auto.html/g" -i /root/configs/nova.conf

cp /root/configs/nova.conf /etc/nova/nova.conf
cp /root/configs/neutron.conf /etc/neutron/neutron.conf
cp /root/configs/ml2_conf.ini /etc/neutron/plugins/ml2/ml2_conf.ini

rm /var/lib/nova/nova.sqlite
service nova-compute restart
service openvswitch-switch restart
ovs-vsctl add-br br-int
service nova-compute restart
service neutron-plugin-openvswitch-agent restart

exit 0