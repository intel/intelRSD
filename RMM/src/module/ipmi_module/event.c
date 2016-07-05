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


#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "ipmi.h"
#include "event.h"

/** socket fd list */
static struct list_head fd_event_head = LIST_HEAD_INIT(fd_event_head);

/**
  *  @brief add socket fd to the monitor socket fd list
  *
  *  @param[in] event include socket fd and handle routine
  */
void fd_event_add(struct fd_event *event)
{
	list_add_tail(&event->list, &fd_event_head);
}

/**
  *  @brief select and receive socket packets from every socket,
  *  then call the corresponding handle routines.
  *
  *  @param
  *  @return
  */
void fd_events_main_loop(void)
{
	int max_fd;
	fd_set rfds;
	struct fd_event *event;

	while (!ipmi_module_exit) {
		FD_ZERO(&rfds);

		max_fd = -1;
		list_for_each_entry(event, &fd_event_head, list) {
			if (event->fd > max_fd)
				max_fd = event->fd;
			if (event->fd == -1)
				continue;
			FD_SET(event->fd, &rfds);
		}

		if (select(max_fd + 1, &rfds, NULL, NULL, NULL) <= 0)
			continue;

		list_for_each_entry(event, &fd_event_head, list) {
			if (event->fd == -1)
				continue;
			if (FD_ISSET(event->fd, &rfds))
				event->handle_fd(event->fd);
		}
	}
}

