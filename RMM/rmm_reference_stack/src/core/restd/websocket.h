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


#ifndef __RMM_RHTTPD_WEBSOCKET_H__
#define __RMM_RHTTPD_WEBSOCKET_H__

#include "http.h"

#define WS_RFC6455_VERSION		13

#define GET_DEVICE_ID_CMD    0x01
#define COLD_RESET_CMD       0x02
#define WARM_RESET_CMD       0x03


#define HTTPD_APP_IPMI_NETFN    0x06


typedef struct mutex_cond {
	pthread_mutex_t mutex_cb;
	pthread_cond_t cond_cb;
} mutex_cond_t, *mutex_cond_p;

#define LEN_GET_DEV_ID_RSP	16
typedef struct get_dev_id_cb_data {
	mutex_cond_t mutex_cond_cb;
	uint8 rsp[LEN_GET_DEV_ID_RSP];
} get_dev_id_cb_data_t, *get_dev_id_cb_data_p;

extern void ws_process(struct http_request *req);

#endif

