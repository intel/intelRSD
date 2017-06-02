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


#ifndef __LIBUTILS_IP_H__
#define __LIBUTILS_IP_H__

int libutils_net_get_ip(const char *ifname, char *ip, int len);
int libutils_net_get_ifname(const char *ip, char *ifname, int len);
int libutils_net_get_gateway(const char *ifname, char *gateway, int len);
int libutils_net_get_origin(const char *ifname, char *origin);
int libutils_net_get_mask(const char *ifname, char *net_mask, int len);
int libutils_netdev_get_attribute(const char *ifname, char *key, char *value, int len);

#endif

