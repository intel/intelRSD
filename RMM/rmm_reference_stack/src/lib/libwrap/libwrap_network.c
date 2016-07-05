/**
 * Copyright (c)  2015, Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *   http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "libwrap.h"


#ifdef RMM_ETH_VIRTUAL
static result_t get_inf_id_map(mngr_eth_inf_id_map_t *inf_id_map)
{
	DIR *dir;
	struct dirent *ptr;
	int rc = 0;
	int8 tmp_ip[64] = {};

	if ((dir=opendir("/sys/class/net")) == NULL) {
		perror("Open dir error...");
		return -1;
	}

	bzero(tmp_ip, sizeof(tmp_ip));

	while ((ptr=readdir(dir)) != NULL) {
		if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..") == 0 || strcmp(ptr->d_name,"lo") == 0)
			continue;
		else if(ptr->d_type == 8 || ptr->d_type == 10) {
			rc = libutils_net_get_ip(ptr->d_name, tmp_ip, sizeof(tmp_ip));
			if (rc == 0) {
				if (strncmp(tmp_ip, "0.0.0.0", strnlen_s(tmp_ip, sizeof(tmp_ip)-1)) != 0 &&
					strncmp(tmp_ip, "127.0.0.1", strnlen_s(tmp_ip, sizeof(tmp_ip)-1)) != 0) {
					strncpy_s(inf_id_map->list[inf_id_map->inf_num].ifname, sizeof(inf_id_map->list[inf_id_map->inf_num].ifname), ptr->d_name, NAME_LEN - 1);
					inf_id_map->list[inf_id_map->inf_num].inf_id = inf_id_map->inf_num + 1;
					inf_id_map->inf_num += 1;
					continue;
				}
			}
		} else if(ptr->d_type == 4)
			continue;
	}
	closedir(dir);
	return RESULT_OK;
}

#else

static result_t get_inf_id_map(mngr_eth_inf_id_map_t *inf_id_map)
{
	DIR *dir;
	struct dirent *ptr;
	int rc = 0;
	int8 tmp_ip[64] = {};

	if ((dir=opendir("/sys/class/net")) == NULL) {
		perror("Open dir error...");
		return -1;
	}

	bzero(tmp_ip, sizeof(tmp_ip));

	while ((ptr=readdir(dir)) != NULL) {
		if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..") == 0 || strcmp(ptr->d_name,"lo") == 0)
			continue;
		else if(ptr->d_type == 8 || ptr->d_type == 10) {
			if (strncmp(ptr->d_name, "eth", 3) == 0) {
				if (strstr(ptr->d_name, ".") == NULL) {
					strncpy_s(inf_id_map->list[inf_id_map->inf_num].ifname, sizeof(inf_id_map->list[inf_id_map->inf_num].ifname), ptr->d_name, NAME_LEN - 1);
					inf_id_map->list[inf_id_map->inf_num].inf_id = inf_id_map->inf_num + 1;
					inf_id_map->inf_num += 1;
					continue;
				}
			}
		} else if(ptr->d_type == 4)
			continue;
	}
	closedir(dir);
	return RESULT_OK;
}

static result_t get_inf_vlans(mngr_eth_inf_id_map_t *inf_id_map)
{
	DIR *dir;
	struct dirent *ptr;
	int rc = 0;
	int8 tmp_ip[64] = {};
	int32 eth_id = 0;
	int32 vlan_id = 0;
	int32 i = 0;
	int8  inf_name[16] = {};

	if ((dir=opendir("/sys/class/net")) == NULL) {
		perror("Open dir error...");
		return -1;
	}

	bzero(tmp_ip, sizeof(tmp_ip));

	while ((ptr=readdir(dir)) != NULL) {
		if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..") == 0 || strcmp(ptr->d_name,"lo") == 0)
			continue;
		else if(ptr->d_type == 8 || ptr->d_type == 10) {
			if (strncmp(ptr->d_name, "eth", 3) == 0) {
				if (strstr(ptr->d_name, ".") != NULL) {
					sscanf(ptr->d_name, "eth%d.%d", &eth_id, &vlan_id);
					snprintf_s_i(inf_name, sizeof(inf_name), "eth%d", eth_id);
					for (i = 0; i < MAX_INF_NUM; i++) {
						if (strcmp(inf_name, inf_id_map->list[i].ifname) == 0) {
							inf_id_map->list[i].vlan_id[inf_id_map->list[i].vlan_num] = vlan_id;
							inf_id_map->list[i].vlan_num += 1;
						}
					}
					continue;
				}
			}
		} else if(ptr->d_type == 4)
			continue;
	}
	closedir(dir);
	return RESULT_OK;
}

#endif

result_t libwrap_manager_eth_inf_coll_init(mngr_eth_inf_coll_t *eth_inf_coll)
{
	int8 buff[16] = {};
	mngr_eth_inf_id_map_t inf_id_map = {};

	if (get_inf_id_map(&inf_id_map) != RESULT_OK)
		return RESULT_ATTR_ERR;

	snprintf_s_s(eth_inf_coll->odata_context, CONTEXT_LEN, "%s", "/redfish/v1/$metadata#Managers/RMC/EthernetInterfaces");
	snprintf_s_s(eth_inf_coll->odata_id, ODATA_ID_LEN, "%s", "/redfish/v1/Managers/RMC/EthernetInterfaces");
	snprintf_s_s(eth_inf_coll->odata_type, ODATA_TYPE_LEN, "%s", "#EthernetInterfaceCollection.1.0.0.EthernetInterfaceCollection");

	snprintf_s_s(eth_inf_coll->id, ID_LEN, "%s", "1");
	snprintf_s_s(eth_inf_coll->name, NAME_LEN, "%s", "Ethernet Network Interface Collection");
	snprintf_s_s(eth_inf_coll->description, DESCRIPTION_LEN, "%s", "Collection of EthernetInterfaces for this Manager");

	eth_inf_coll->inf_num = inf_id_map.inf_num;

	return RESULT_OK;
}


result_t libwrap_pack_manager_eth_inf_coll_json(json_t *result, mngr_eth_inf_coll_t *eth_inf_coll)
{
	int32 i = 0;
	int8 *members = NULL;
	int8 buff[HREF_URL_LEN] = {};
	mngr_eth_inf_id_map_t inf_id_map = {};

	add_json_string(result, RMM_JSON_ODATA_CONTEXT, eth_inf_coll->odata_context);
	add_json_string(result, RMM_JSON_ODATA_ID, eth_inf_coll->odata_id);
	add_json_string(result, RMM_JSON_ODATA_TYPE, eth_inf_coll->odata_type);
	add_json_string(result, RMM_JSON_NAME, eth_inf_coll->name);
	add_json_string(result, RMM_JSON_DESC, eth_inf_coll->description);

	members = (int8 *)malloc(eth_inf_coll->inf_num * HREF_URL_LEN);
	if (members == NULL)
		return RESULT_MALLOC_ERR;

	if (get_inf_id_map(&inf_id_map) != RESULT_OK)
		return RESULT_ATTR_ERR;

	memset(members, 0, eth_inf_coll->inf_num * HREF_URL_LEN);
	for (i = 0; i < eth_inf_coll->inf_num; i++) {
		snprintf_s_si(buff, HREF_URL_LEN, "%s/%d", eth_inf_coll->odata_id, inf_id_map.list[i].inf_id);
		memcpy_s(members + i * HREF_URL_LEN, HREF_URL_LEN, buff, HREF_URL_LEN);
	}

	json_add_multi_member(result, eth_inf_coll->inf_num, members);

	if (members)
		free(members);

	return RESULT_OK;
}

result_t libwrap_manager_eth_inf_init(mngr_eth_inf_t *eth_inf, int32 inf_id)
{
	int8 buff[16] = {};
	int8 ifname[16] = {};
	mngr_eth_inf_id_map_t inf_id_map = {};
	int32 i = 0;

	if (eth_inf == NULL)
		return RESULT_NONE_POINTER;

	if (inf_id <= 0)
		return RESULT_NO_NODE;

	snprintf_s_s(eth_inf->odata_context, CONTEXT_LEN, "%s", "/redfish/v1/$metadata#Managers/RMC/EthernetInterfaces/Members/$entity");
	snprintf_s_i(eth_inf->odata_id, ODATA_ID_LEN, "/redfish/v1/Managers/RMC/EthernetInterfaces/%d", inf_id);
	snprintf_s_s(eth_inf->odata_type, ODATA_TYPE_LEN, "%s", "#EthernetInterface.1.0.0.EthernetInterface");

	snprintf_s_i(eth_inf->id, ID_LEN, "%d", inf_id);
	snprintf_s_s(eth_inf->name, NAME_LEN, "%s", "Manager Ethernet Interface");
	snprintf_s_s(eth_inf->description, DESCRIPTION_LEN, "%s", "Management Network Interface");
	snprintf_s_s(eth_inf->link_tech, NAME_LEN, "%s", "Ethernet");

	if (get_inf_id_map(&inf_id_map) != RESULT_OK)
		return RESULT_ATTR_ERR;

	for (i = 0; i < MAX_INF_NUM; i++) { if (inf_id_map.list[i].inf_id == inf_id) {
			strncpy_safe(ifname, inf_id_map.list[i].ifname, sizeof(ifname), strnlen_s(inf_id_map.list[i].ifname, sizeof(inf_id_map.list[i].ifname)-1));
			break;
		}
	}

	if (i== MAX_INF_NUM)
		return RESULT_NO_NODE;

	libutils_netdev_get_attribute(ifname, "address", eth_inf->factory_mac, MAC_ADDR_LEN);
	libutils_netdev_get_attribute(ifname, "address", eth_inf->mac_addr, MAC_ADDR_LEN);

	libutils_netdev_get_attribute(ifname, "speed", buff, sizeof(buff));
	eth_inf->speed_mbps = str2int(buff);
	eth_inf->auto_sense = true;
	libutils_netdev_get_attribute(ifname, "duplex", buff, sizeof(buff));
	if (strcmp(buff, "full") == 0)
		eth_inf->full_duplex = true;

	libutils_netdev_get_attribute(ifname, "mtu", buff, sizeof(buff));
	eth_inf->frame_size = str2int(buff);
	eth_inf->max_ipv6_static_num = 1;

	eth_inf->vlan_enable = false;

	libutils_net_get_ip(ifname, eth_inf->ipv4_addr.address, IPV4_ADDR_LEN);
	libutils_net_get_mask(ifname, eth_inf->ipv4_addr.subnet_mask, IPV4_ADDR_LEN);
	snprintf_s_s(eth_inf->ipv4_addr.addr_origin, NAME_LEN, "%s", "DHCP");
	libutils_net_get_gateway(ifname, eth_inf->ipv4_addr.gateway, IPV4_ADDR_LEN);

	snprintf_s_i(eth_inf->vlans, URL_LEN, "/redfish/v1/Managers/RMC/EthernetInterfaces/%d/VLANs", inf_id);

	return RESULT_OK;
}

static result_t libwrap_pack_vlan_inf_ipv4_addr(json_t *ipv4_array, int32 inf_id)
{
	json_t *ipv4_addr = NULL;
	int8 ifname[16] = {};
	int8 buff[16] = {};
	mngr_eth_inf_id_map_t inf_id_map = {};
	mngr_eth_inf_t eth_inf = {};
	int32 vlan_id;
	int32 i;

	if (get_inf_id_map(&inf_id_map) != RESULT_OK)
		return RESULT_ATTR_ERR;

	if (get_inf_vlans(&inf_id_map) != RESULT_OK)
		return RESULT_ATTR_ERR;

	for (i = 0; i < inf_id_map.list[inf_id - 1].vlan_num; i++) {
		vlan_id = inf_id_map.list[inf_id - 1].vlan_id[i];
		snprintf_s_ii(ifname, sizeof(ifname) - 1, "eth%d.%d", (inf_id - 1), vlan_id);
		libutils_net_get_ip(ifname, eth_inf.ipv4_addr.address, IPV4_ADDR_LEN);
		libutils_net_get_mask(ifname, eth_inf.ipv4_addr.subnet_mask, IPV4_ADDR_LEN);
		if ((strlen(eth_inf.ipv4_addr.address) == 0) || (strlen(eth_inf.ipv4_addr.subnet_mask) == 0))
			continue;
		snprintf_s_s(eth_inf.ipv4_addr.addr_origin, NAME_LEN, "%s", "DHCP");
		libutils_net_get_gateway(ifname, eth_inf.ipv4_addr.gateway, IPV4_ADDR_LEN);

		ipv4_addr = json_object();
		if (ipv4_addr != NULL) {
			add_json_string(ipv4_addr, RMM_JSON_MANAGER_IP_ADDR, eth_inf.ipv4_addr.address);
			add_json_string(ipv4_addr, RMM_JSON_MANAGER_SUBNET_MASK, eth_inf.ipv4_addr.subnet_mask);
			add_json_string(ipv4_addr, RMM_JSON_MANAGER_ADDR_ORIGIN, eth_inf.ipv4_addr.addr_origin);
			add_json_string(ipv4_addr, RMM_JSON_MANAGER_IPV4_GATEWAY, eth_inf.ipv4_addr.gateway);
		}
		json_array_add(ipv4_array, ipv4_addr);
	}

	return RESULT_OK;
}

result_t libwrap_pack_manager_eth_inf_json(json_t *result, mngr_eth_inf_t *eth_inf)
{
	json_t *status = NULL;
	json_t *ipv4_array = NULL;
	json_t *ipv4_addr = NULL;
	json_t *ipv6_policy = NULL;
	json_t *ipv6_policy_array = NULL;
	json_t *ipv6_static = NULL;
	json_t *ipv6_static_array = NULL;
	json_t *ipv6_addr = NULL;
	json_t *ipv6_addr_array = NULL;
	json_t *name_servers = NULL;
	json_t *vlans = NULL;
	int32 i = 0;

	add_json_string(result, RMM_JSON_ODATA_CONTEXT, eth_inf->odata_context);
	add_json_string(result, RMM_JSON_ODATA_ID, eth_inf->odata_id);
	add_json_string(result, RMM_JSON_ODATA_TYPE, eth_inf->odata_type);
	add_json_string(result, RMM_JSON_ID, eth_inf->id);
	add_json_string(result, RMM_JSON_NAME, eth_inf->name);
	add_json_string(result, RMM_JSON_DESC, eth_inf->description);

	status = json_object();
	if (status != NULL) {
		add_json_string(status, RMM_JSON_MANAGER_STATE, RMM_JSON_STATE_ENABLED);
		add_json_string(status, RMM_JSON_MANAGER_HEALTH, "OK");
	}
	json_object_add(result, RMM_JSON_STATUS, status);

	add_json_string(result, RMM_JSON_MANAGER_FACTORY_MAC, eth_inf->factory_mac);
	add_json_string(result, RMM_JSON_MANAGER_MAC, eth_inf->mac_addr);
	add_json_integer(result, RMM_JSON_MANAGER_SPD_MBPS , eth_inf->speed_mbps);
	add_json_bool(result, RMM_JSON_MANAGER_AUTO_SENSE, eth_inf->auto_sense);
	add_json_bool(result, RMM_JSON_MANAGER_FULL_DUPLEX , eth_inf->full_duplex);
	add_json_integer(result, RMM_JSON_MANAGER_FRAME_SIZE, eth_inf->frame_size);
	add_json_string(result, RMM_JSON_MANAGER_NETWORK_HOSTNAME, eth_inf->hostname);
	add_json_string(result, RMM_JSON_MANAGER_NETWORK_FQDN, eth_inf->fqdn);
	add_json_integer(result, RMM_JSON_MANAGER_MAX_IPV6_ADDR, eth_inf->max_ipv6_static_num);

	ipv4_array = json_array();
	if (ipv4_array == NULL)
		return RESULT_NONE_POINTER;

	if (strlen(eth_inf->ipv4_addr.address) != 0) {
		ipv4_addr = json_object();
		if (ipv4_addr != NULL) {
			add_json_string(ipv4_addr, RMM_JSON_MANAGER_IP_ADDR, eth_inf->ipv4_addr.address);
			add_json_string(ipv4_addr, RMM_JSON_MANAGER_SUBNET_MASK, eth_inf->ipv4_addr.subnet_mask);
			add_json_string(ipv4_addr, RMM_JSON_MANAGER_ADDR_ORIGIN, eth_inf->ipv4_addr.addr_origin);
			add_json_string(ipv4_addr, RMM_JSON_MANAGER_IPV4_GATEWAY, eth_inf->ipv4_addr.gateway);
		} 
		json_array_add(ipv4_array, ipv4_addr);
	}

	libwrap_pack_vlan_inf_ipv4_addr(ipv4_array, atoi(eth_inf->id));
	json_object_add(result, RMM_JSON_MANAGER_IPV4_ADDR, ipv4_array);

	ipv6_policy_array = json_array();
	if (ipv6_policy_array == NULL)
		return RESULT_NONE_POINTER;

	ipv6_policy = json_object();
	if (ipv6_policy != NULL) {
		add_json_string(ipv6_policy, RMM_JSON_MANAGER_IPV6_PREFIX, eth_inf->ipv6_policy.prefix);
		add_json_integer(ipv6_policy, RMM_JSON_MANAGER_IPV6_PRECEDENCE, eth_inf->ipv6_policy.precedence);
		add_json_integer(ipv6_policy, RMM_JSON_MANAGER_IPV6_LABEL, eth_inf->ipv6_policy.label);
	}
	if (strnlen_s(eth_inf->ipv6_policy.prefix, sizeof(eth_inf->ipv6_policy.prefix)-1) != 0)
		json_array_add(ipv6_policy_array, ipv6_policy);
	json_object_add(result, RMM_JSON_MANAGER_IPV6_POLICY, ipv6_policy_array);


	ipv6_static_array = json_array();
	if (ipv6_static_array == NULL)
		return RESULT_NONE_POINTER;

	ipv6_static = json_object();
	if (ipv6_static != NULL) {
		add_json_string(ipv6_static, RMM_JSON_MANAGER_IP_ADDR, eth_inf->ipv6_static.address);
		add_json_integer(ipv6_static, RMM_JSON_MANAGER_IPV6_PREFIX_LEN, eth_inf->ipv6_static.prefix_len);
		add_json_string(ipv6_static, RMM_JSON_MANAGER_ADDR_ORIGIN, eth_inf->ipv6_static.addr_origin);
		add_json_string(ipv6_static, RMM_JSON_MANAGER_IPV6_ADDR_STATE, eth_inf->ipv6_static.addr_state);
	}
	if (strnlen_s(eth_inf->ipv6_static.address, sizeof(eth_inf->ipv6_static.address)-1) != 0)
		json_array_add(ipv6_static_array, ipv6_static);
	json_object_add(result, RMM_JSON_MANAGER_IPV6_STATIC, ipv6_static_array);


	ipv6_addr_array = json_array();
	if (ipv6_addr_array == NULL)
		return RESULT_NONE_POINTER;

	ipv6_addr = json_object();
	if (ipv6_static != NULL) {
		add_json_string(ipv6_addr, RMM_JSON_MANAGER_IP_ADDR, eth_inf->ipv6_addr.address);
		add_json_integer(ipv6_addr, RMM_JSON_MANAGER_IPV6_PREFIX_LEN, eth_inf->ipv6_addr.prefix_len);
		add_json_string(ipv6_addr, RMM_JSON_MANAGER_ADDR_ORIGIN, eth_inf->ipv6_addr.addr_origin);
		add_json_string(ipv6_addr, RMM_JSON_MANAGER_IPV6_ADDR_STATE, eth_inf->ipv6_addr.addr_state);
	}
	if (strnlen_s(eth_inf->ipv6_addr.address, sizeof(eth_inf->ipv6_addr.address)-1) != 0)
		json_array_add(ipv6_addr_array, ipv6_addr);
	json_object_add(result, RMM_JSON_MANAGER_IPV6_ADDR, ipv6_addr_array);

	add_json_string(result, RMM_JSON_MANAGER_IPV6_GATEWAY, eth_inf->ipv6_gateway);

	name_servers = json_array();
	if (name_servers != NULL) {
		for (i = 0; i < MAX_NAME_SERVER; i ++) {
			if (strnlen_s(eth_inf->name_servers[i], sizeof(eth_inf->name_servers[i])-1) > 0)
				json_array_add(name_servers, json_string(eth_inf->name_servers[i]));
		}
	}
	json_object_add(result, RMM_JSON_MANAGER_NAME_SERVERS, name_servers);

	vlans = json_object();
	if (vlans != NULL) {
		add_json_string(vlans, RMM_JSON_ODATA_ID, eth_inf->vlans);
	}
	json_object_add(result, RMM_JSON_MANAGER_VLANS, vlans);

	return RESULT_OK;
}


result_t libwrap_manager_network_srv_init(mngr_network_srv_t *network_srv)
{
	int https_port;
	if (network_srv == NULL)
		return RESULT_NONE_POINTER;

	get_stunnel_ports(&https_port);

	snprintf_s_s(network_srv->odata_context, CONTEXT_LEN, "%s", "/redfish/v1/$metadata#Managers/RMC/NetworkProtocol/$entity");
	snprintf_s_s(network_srv->odata_id, ODATA_ID_LEN, "%s", "/redfish/v1/Managers/RMC/NetworkProtocol");
	snprintf_s_s(network_srv->odata_type, ODATA_TYPE_LEN, "%s", "#ManagerNetworkProtocol.1.0.0.ManagerNetworkProtocol");

	snprintf_s_s(network_srv->id, ID_LEN, "%s", "NetworkProtocol");
	snprintf_s_s(network_srv->name, NAME_LEN, "%s", "Manager Network Protocol");
	snprintf_s_s(network_srv->description, DESCRIPTION_LEN, "%s", "Manager Network Service");

	network_srv->http.enabled = true;
	network_srv->http.port = RACK_HTTPD_PORT;

	network_srv->https.enabled = true;
	if (https_port == -1)
		https_port = RACK_HTTPSD_PORT;
	network_srv->https.port = https_port;

	network_srv->ssh.enabled = true;
	network_srv->ssh.port = 22;

	return RESULT_OK;
}

result_t libwrap_pack_manager_network_srv_json(json_t *result, mngr_network_srv_t *network_srv)
{
	json_t *status = NULL;
	json_t *http = NULL;
	json_t *https = NULL;
	json_t *ssh = NULL;

	add_json_string(result, RMM_JSON_ODATA_CONTEXT, network_srv->odata_context);
	add_json_string(result, RMM_JSON_ODATA_ID, network_srv->odata_id);
	add_json_string(result, RMM_JSON_ODATA_TYPE, network_srv->odata_type);
	add_json_string(result, RMM_JSON_ID, network_srv->id);
	add_json_string(result, RMM_JSON_NAME, network_srv->name);
	add_json_string(result, RMM_JSON_DESC, network_srv->description);
	add_json_string(result, RMM_JSON_MANAGER_NETWORK_HOSTNAME, network_srv->hostname);
	add_json_string(result, RMM_JSON_MANAGER_NETWORK_FQDN, network_srv->fqdn);

	status = json_object();
	if (status != NULL) {
		add_json_string(status, RMM_JSON_MANAGER_STATE, RMM_JSON_STATE_ENABLED);
		add_json_string(status, RMM_JSON_MANAGER_HEALTH, "OK");
	}
	json_object_add(result, RMM_JSON_STATUS, status);

	http = json_object();
	if (http != NULL) {
		add_json_bool(http, RMM_JSON_PROTOCOL_ENABLED, network_srv->http.enabled);
		add_json_integer(http, RMM_JSON_PORT, network_srv->http.port);
	}
	json_object_add(result, RMM_JSON_MANAGER_NETWORK_HTTP, http);

	https = json_object();
	if (https != NULL) {
		add_json_bool(https, RMM_JSON_PROTOCOL_ENABLED, network_srv->https.enabled);
		add_json_integer(https, RMM_JSON_PORT, network_srv->https.port);
	}
	json_object_add(result, RMM_JSON_MANAGER_NETWORK_HTTPS, https);

	ssh = json_object();
	if (ssh != NULL) {
		add_json_bool(ssh, RMM_JSON_PROTOCOL_ENABLED, network_srv->ssh.enabled);
		add_json_integer(ssh, RMM_JSON_PORT, network_srv->ssh.port);
	}
	json_object_add(result, RMM_JSON_MANAGER_NETWORK_SSH, ssh);

	return RESULT_OK;
}

void update_href_host(int8 *href, int32 len, int8 *host)
{
	int32 ip[4] = {};
	int32 port = 0;
	char url[96] = {};

	sscanf(href, "http://%d.%d.%d.%d:%d%s", &ip[0],&ip[1],&ip[2],&ip[3], &port, url);
	snprintf_s_ss(href, len, "http://%s%s", host, url);
}


result_t libwrap_manager_vlans_init(mngr_vlans_t *vlans, int32 inf_id)
{
	int8 buff[16] = {};
	mngr_eth_inf_id_map_t inf_id_map = {};
	int i = 0;

	if (get_inf_id_map(&inf_id_map) != RESULT_OK)
		return RESULT_ATTR_ERR;
 
#ifndef RMM_ETH_VIRTUAL
	if (get_inf_vlans(&inf_id_map) != RESULT_OK)
		return RESULT_ATTR_ERR;
#endif

	for (i = 0; i < MAX_INF_NUM; i++) {
		if (inf_id_map.list[i].inf_id == inf_id) {
			break;
		}
	}

	if (i== MAX_INF_NUM)
		return RESULT_NO_NODE;

	snprintf_s_i(vlans->odata_context, CONTEXT_LEN, "/redfish/v1/$metadata#Managers/RMC/EthernetInterfaces/%d/VLANs", inf_id);
	snprintf_s_i(vlans->odata_id, ODATA_ID_LEN, "/redfish/v1/Managers/RMC/EthernetInterfaces/%d/VLANs", inf_id);
	snprintf_s_s(vlans->odata_type, ODATA_TYPE_LEN, "%s", "#VLanNetworkInterfaceCollection.1.0.0.VLanNetworkInterfaceCollection");
	snprintf_s_s(vlans->name, NAME_LEN, "%s", "VLAN Collection");

	vlans->num = inf_id_map.list[inf_id - 1].vlan_num;

	for (i = 0; i < vlans->num; i++) {
		snprintf_s_ii(vlans->url[i], URL_LEN, "/redfish/v1/Managers/RMC/EthernetInterfaces/%d/VLANs/%d", inf_id, i + 1);
	}

	return RESULT_OK;
}


result_t libwrap_pack_manager_vlans_json(json_t *result, mngr_vlans_t *vlans)
{
	json_t *links = NULL;
	json_t *array = NULL;
	json_t *member = NULL;
	int8   buf[128] = {};
	int32 i = 0;

	add_json_string(result, RMM_JSON_ODATA_CONTEXT, vlans->odata_context);
	add_json_string(result, RMM_JSON_ODATA_ID, vlans->odata_id);
	add_json_string(result, RMM_JSON_ODATA_TYPE, vlans->odata_type);
	add_json_string(result, RMM_JSON_NAME, vlans->name);

	add_json_integer(result, RMM_JSON_MEMBERS_ODATA_COUNT, vlans->num);
	array = json_array();
	if (array) {
		for (i = 0; i < vlans->num; i++) {
			member = json_object();
			if (member != NULL) {
				add_json_string(member, RMM_JSON_ODATA_ID, vlans->url[i]);
				json_array_add(array, member);
			}
		}
		json_object_add(result, RMM_JSON_MEMBERS, array);
	}

	json_object_add(result, RMM_JSON_RF_LINKS, links);

	return RESULT_OK;

}

result_t libwrap_manager_vlan_init(mngr_vlan_t *vlan, int32 inf_id, int32 vlan_idx)
{
	int8 buff[16] = {};
	mngr_eth_inf_id_map_t inf_id_map = {};
	int i = 0;

	if (get_inf_id_map(&inf_id_map) != RESULT_OK)
		return RESULT_ATTR_ERR;
 
	if (get_inf_vlans(&inf_id_map) != RESULT_OK)
		return RESULT_ATTR_ERR;

	if (vlan_idx > inf_id_map.list[inf_id - 1].vlan_num || vlan_idx < 1)
		return RESULT_ATTR_ERR;

	snprintf_s_ii(vlan->odata_context, CONTEXT_LEN, "/redfish/v1/$metadata#Managers/RMC/EthernetInterfaces/%d/VLANs/%d", inf_id, vlan_idx);
	snprintf_s_ii(vlan->odata_id, ODATA_ID_LEN, "/redfish/v1/Managers/RMC/EthernetInterfaces/%d/VLANs/%d", inf_id, vlan_idx);
	snprintf_s_s(vlan->odata_type, ODATA_TYPE_LEN, "%s", "#VLanNetworkInterface.1.0.0.VLanNetworkInterface");
	snprintf_s_i(vlan->id, ID_LEN, "VLAN%d", vlan_idx);
	snprintf_s_s(vlan->name, NAME_LEN, "%s", "VLAN Network Interface");
	snprintf_s_i(vlan->description, DESCRIPTION_LEN, "System NIC %d VLAN", inf_id);

	vlan->enable = true;
	vlan->vlan_id = inf_id_map.list[inf_id - 1].vlan_id[vlan_idx - 1];
	vlan->oem.tagged = false;
	snprintf_s_s(vlan->oem.odata_type, ODATA_TYPE_LEN, "%s", "http://rsa.intel.com/Schema#RSA.VLanNetworkInterface");

	return RESULT_OK;
}


result_t libwrap_pack_manager_vlan_json(json_t *result, mngr_vlan_t *vlan)
{
	json_t *links = NULL;
	json_t *array = NULL;
	json_t *member = NULL;
	json_t *status = NULL;
	json_t *oem = NULL;
	json_t *rsa = NULL;
	int8   buf[128] = {};
	int32 i = 0;

	add_json_string(result, RMM_JSON_ODATA_CONTEXT, vlan->odata_context);
	add_json_string(result, RMM_JSON_ODATA_ID, vlan->odata_id);
	add_json_string(result, RMM_JSON_ODATA_TYPE, vlan->odata_type);

	add_json_string(result, RMM_JSON_ID, vlan->id);
	add_json_string(result, RMM_JSON_NAME, vlan->name);
	add_json_string(result, RMM_JSON_DESC, vlan->description);

	status = json_object();
	if (status != NULL) {
		add_json_string(status, RMM_JSON_MANAGER_STATE, RMM_JSON_STATE_ENABLED);
		add_json_string(status, RMM_JSON_MANAGER_HEALTH, "OK");
	}
	json_object_add(result, RMM_JSON_STATUS, status);

	add_json_bool(result, RMM_JSON_MANAGER_VLAN_ENABLE, vlan->enable);
	add_json_integer(result, RMM_JSON_MANAGER_VLAN_ID, vlan->vlan_id);

	oem = json_object();
	if (oem != NULL) {
		rsa = json_object();
		if (rsa != NULL) {
			add_json_string(rsa, RMM_JSON_ODATA_TYPE, vlan->oem.odata_type);
			add_json_bool(rsa, RMM_JSON_MANAGER_VLAN_TAGGED, vlan->oem.tagged);
		}
		json_object_add(oem, RMM_JSON_OEM_INTEL_RSA, rsa);
		json_object_add(result, RMM_JSON_OEM, oem);
	}

	return RESULT_OK;

}


