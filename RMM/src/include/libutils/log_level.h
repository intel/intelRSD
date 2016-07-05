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


#ifndef __LIBUTILS_LOG_LEVEL_H__
#define __LIBUTILS_LOG_LEVEL_H__

#define EVT_SEVERITY_LEN		(32)
#define EVT_MODULE_LEN			(32)

#define EVT_FN_LEN				(128)
#define EVT_MSG_LEN				(1024)

#define EVT_WRITE_LOG 			0

enum log_level {
	CRITICAL,
	ERROR,
	WARNING,
	INFO,
	DBG,
	LEVEL_MAX,
};

struct log_evt {
	int		type;
	char 	module_name[EVT_MODULE_LEN];
	int 	level;
	char	fn_name[EVT_FN_LEN];
	char	msg[EVT_MSG_LEN];
};

#endif
