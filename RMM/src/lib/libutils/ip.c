/**
 * Copyright (c)  2015-2017 Intel Corporation.
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


#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <dirent.h>

#include <errno.h>
#include "libutils/string.h"
#include "libsecurec/safe_lib.h"



int libutils_net_get_ip(const char *ifname, char *ip, int len)
{
	int sockfd;
	struct ifreq ifr;

	bzero(ip, len);

	strncpy_s(ifr.ifr_name, IFNAMSIZ, ifname, IFNAMSIZ-1);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(ioctl(sockfd, SIOCGIFADDR, &ifr) == -1) {
		close(sockfd);
		return -1;
	}

	struct sockaddr_in *myaddr;
	myaddr = (struct sockaddr_in*)&(ifr.ifr_addr);
	strncpy_s(ip, len, inet_ntoa(myaddr->sin_addr), len - 1);
	close(sockfd);
	return 0;
}



int libutils_net_get_ifname(const char *ip, char *ifname, int len)
{
	DIR *dir;
	struct dirent *ptr;
	int rc = 0;
	int8 tmp_ip[64];
	int8 ip_addr[16];;

	bzero(ip_addr, sizeof(ip_addr));
	if(strstr(ip, ":")) {
		sscanf(ip, "%15[^:]:%*s", ip_addr);
	}

	if ((dir=opendir("/sys/class/net")) == NULL) {
		perror("Open dir error...");
		return -1;
	}

	bzero(tmp_ip, sizeof(tmp_ip));
	bzero(ifname, len);

	while ((ptr=readdir(dir)) != NULL) {
		if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..") == 0)
			continue;
		else if(ptr->d_type == 8 || ptr->d_type == 10) {
			rc = libutils_net_get_ip(ptr->d_name, tmp_ip, sizeof(tmp_ip));
			if (rc == 0) {
				if (strncmp(tmp_ip, ip_addr, strnlen_s(ip_addr, sizeof(ip_addr)-1)) == 0) {
					strncpy_s(ifname, len, ptr->d_name, len - 1);
					closedir(dir);
					return 0;
				}
			}
		} else if(ptr->d_type == 4)
			continue;
	}
	closedir(dir);
	return -1;
}

int libutils_net_get_gateway(const char *ifname, char *gateway, int len)
{
	FILE *fp;
	int8 buf[256];
	int8 iface[16];
	uint dest_addr, gw;

	fp = fopen("/proc/net/route", "r");
	if (fp == NULL)
		return -1;

	bzero(buf, sizeof(buf));
	bzero(iface, sizeof(iface));
	bzero(gateway, len);

	/* Skip the title line */
	fgets(buf, sizeof(buf), fp);

	while (fgets(buf, sizeof(buf), fp)) {
		if (sscanf(buf, "%15s\t%X\t%X", iface, &dest_addr, &gw) == 3) {
			if (strncmp(iface, ifname, strnlen_s(ifname, RSIZE_MAX_STR)) == 0) {
				snprintf_s_iiii(gateway, 16, "%d.%d.%d.%d", gw&0xff, (gw>>8)&0xff, (gw>>16)&0xff, (gw>>24)&0xff);
				if (strncmp(gateway, "0.0.0.0", strnlen_s("0.0.0.0", RSIZE_MAX_STR)) == 0)
					continue;
				else
					break;
			}
		}
	}

	fclose(fp);
	return 0;
}

int libutils_net_get_origin(const char *ifname, char *origin) {
	FILE *fp;
	char line[256];
	const char origin_dhcp[] = "DHCP";
	const char origin_static[] = "Static";

	fp = fopen("/etc/network/interfaces", "r");
	if (fp == NULL)
		return -1;

	while (fgets(line, sizeof(line), fp)) {
		char  iface_str[65] = {0};
		char      iface[65] = {0};
		char   inet_str[65] = {0};
		char origin_str[65] = {0};
		if (sscanf(line, "%64s %64s %64s %64s",
			   iface_str, iface, inet_str, origin_str) == 4) {
			if (strncmp(iface, ifname, strlen(ifname)) == 0 &&
				strncmp(iface_str, "iface", strlen(ifname)) == 0 &&
				strncmp(inet_str, "inet", strlen(ifname)) == 0) {
				if (strncmp(origin_str, "static", strnlen_s(origin_str, RSIZE_MAX_STR)) == 0) {
					strncpy_s(origin, sizeof(origin_static), origin_static, sizeof(origin_static));
					fclose(fp);
					return 0;
				}
			}
		}
	}
	//Let`s assume that DHCP is the default value
	strncpy_s(origin, sizeof(origin_dhcp), origin_dhcp, sizeof(origin_dhcp));
	fclose(fp);
	return 0;
}

int libutils_net_get_mask(const char *ifname, char *net_mask, int len)
{
	FILE *fp;
	int8 buf[256];
	int8 iface[16];
	unsigned int dest_addr = 0, mask = 0, tmp = 0;

	fp = fopen("/proc/net/route", "r");
	if (fp == NULL)
		return -1;

	bzero(buf, sizeof(buf));
	bzero(iface, sizeof(iface));
	bzero(net_mask, len);

	/* Skip title line */
	fgets(buf, sizeof(buf), fp);

	while (fgets(buf, sizeof(buf), fp)) {
		if (sscanf(buf, "%15s\t%X\t%X\t%X\t%X\t%X\t%X\t%X", iface, &dest_addr, &tmp, &tmp, &tmp, &tmp, &tmp, &mask) == 8) {
			if (strncmp(iface, ifname, strnlen_s(ifname, RSIZE_MAX_STR)) == 0) {
				snprintf_s_iiii(net_mask, len, "%d.%d.%d.%d",mask&0xff, (mask>>8)&0xff, (mask>>16)&0xff, (mask>>24)&0xff);
				if (strncmp(net_mask, "0.0.0.0", strnlen_s("0.0.0.0", RSIZE_MAX_STR)) == 0)
					continue;
				else
					break;
			}
		}
	}

	fclose(fp);
	return 0;
}

int libutils_netdev_get_attribute(const char *ifname, char *key, char *value, int len)
{
	FILE *fp = NULL;
	int8 path[256];
	int8 tmp_value[256];

	if (len > sizeof(tmp_value))
		return -1;

	bzero(path, sizeof(path));
	bzero(tmp_value, sizeof(tmp_value));
	bzero(value, len);

	snprintf_s_ss(path, sizeof(path), "/sys/class/net/%s/%s", (char *)ifname, key);

	fp = fopen(path, "r");
	if (fp) {
		if (fread(tmp_value, sizeof(tmp_value), 1, fp) >= 0) {
			fclose(fp);
			int ct = strnlen_s(tmp_value, sizeof(tmp_value)-1);
			if (tmp_value[ct - 1] == '\n')
				tmp_value[ct - 1] = '\0';
			//strncpy(value, tmp_value, len);
			memcpy_s(value, len, tmp_value, len);
			return 0;
		}
		fclose(fp);
	} else {
		perror("open");
	}

	return -1;
}
