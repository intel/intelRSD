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


#ifndef __IPMI_EVENT_H__
#define __IPMI_EVENT_H__

#include "libutils/list.h"

struct fd_event {
	struct list_head list;

	int fd;

	void (*handle_fd)(int sockfd);
};

void fd_event_add(struct fd_event *event);
void fd_events_main_loop(void);

#endif

