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


#ifndef __LIBUTILS_UPGRADE_H__
#define __LIBUTILS_UPGRADE_H__

#define MAX_DEB_NAME_LEN	64
#define MAX_DEB_PATH_LEN	64

#define UPGRADE_PREPARE			1
#define UPGRADE_PREPARE_ACK		2
#define UPGRADE_CONFIRM			3
#define SERVICE_RESET_PREPARE	4
#define FACTORY_DEFAULT_PREPARE	5

struct _upgrade_monitor {
	unsigned char sync;
	unsigned int seq;
	unsigned char deb_name[MAX_DEB_NAME_LEN];
	unsigned char deb_path[MAX_DEB_PATH_LEN];
};

#endif
