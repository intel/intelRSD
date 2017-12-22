#!/usr/bin/env python
import subprocess
import os
import time

VLAN_ID = "4088"
RETRIES = 5
OPERSTATE_UP = "up\n"
INTERFACES_DIR = "/sys/class/net/"
REST_SERVER = "/usr/bin/psme-rest-server"
REST_SERVER_CONFIG = "/etc/psme/psme-rest-server-configuration.json"
COMPUTE_EMULATOR = "/usr/bin/psme-compute-simulator"
COMPUTE_EMULATOR_CONFIG = "/etc/psme/psme-compute-simulator-configuration.json"
INTERFACE_TO_REPLACE = "enp0s20f0.4094"

def check_addr(netdev):
	LINK_LOCAL_BRD = "169.254.255.255"
	p = subprocess.Popen("ip addr show " + netdev, shell = True, stdout = subprocess.PIPE)
	data = p.communicate()
	sdata = data[0].split('\n')
	brd = sdata[2].strip().split(' ')[3]
	if LINK_LOCAL_BRD == brd:
			return False
	return True

def setup_network():
       interfaces_all = os.listdir(INTERFACES_DIR)
       interfaces_filtered = [item for item in interfaces_all if item != "lo" and item != "sit0"]
       retries_left = RETRIES
       for interface in interfaces_filtered:
                                   with open(INTERFACES_DIR + interface + "/operstate") as f:
                                                 operstate = f.readline()
                                   if operstate != OPERSTATE_UP:
                                                 subprocess.call( "ifconfig " + interface + " up", shell = True)

       while retries_left > 0:
                     retries_left -= 1
                     connected = False
                     for interface in interfaces_filtered:
                                   netdev = interface + "." + VLAN_ID
                                   subprocess.call( "ip link add link " + interface + " name " + netdev + " type vlan id " + VLAN_ID , shell = True)
                                   result = subprocess.call("dhcpcd " + netdev , shell = True)
                                   if result == 0 and check_addr(netdev):
                                                 connected = True
                                                 break
                                   subprocess.call( "ip link delete " + netdev , shell = True)
                     if connected:
                                   ping_dhcp_srv(netdev)
                                   start_agents(netdev)
                                   break

def ping_dhcp_srv(netdev):
	# Pinging DHCP Server to fill ARP table, otherwise DHCP RELEASE might not be send
	p = subprocess.Popen("dhcpcd -U " + netdev + " | grep dhcp_server_identifier | cut -d \"'\" -f 2", shell = True, stdout = subprocess.PIPE)
	data = p.communicate()
	srv_ip = data[0].split('\n')[0]
	retries_left = RETRIES
	while os.system("ping -c 1 " + srv_ip) !=0 and retries_left > 0:
		time.sleep(1)
		retries_left -= 1

def start_agents(netdev):
	new_env = os.environ.copy()
	new_env['LD_LIBRARY_PATH']='/usr/local/lib'
	subprocess.call(['sed', '-i', r"s/\"" + INTERFACE_TO_REPLACE + "\"/\"" + netdev + "\"/g", REST_SERVER_CONFIG])
	subprocess.Popen([REST_SERVER, REST_SERVER_CONFIG], env=new_env)
	subprocess.Popen([COMPUTE_EMULATOR, COMPUTE_EMULATOR_CONFIG], env=new_env)

def main():
	setup_network()

if __name__ == '__main__':
	main()
