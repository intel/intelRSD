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


#include <stdio.h>   
#include <stdlib.h>  
#include <sys/resource.h>
#include <stdlib.h>
#include "libutils/dump.h"

void enable_core_dump(void)
{
	struct rlimit rlim_new, rlim;
	int ret;

	if (getrlimit(RLIMIT_CORE, &rlim) == 0) {
		rlim_new.rlim_cur = rlim_new.rlim_max = RLIM_INFINITY;
		if (setrlimit(RLIMIT_CORE, &rlim_new) != 0) {
			rlim_new.rlim_cur = rlim_new.rlim_max = rlim.rlim_max;
			(void) setrlimit(RLIMIT_CORE, &rlim_new);
		}
	}
	ret = system("echo \"/var/core.%e.%p.%h.%t\" > /proc/sys/kernel/core_pattern");
	if(ret == -1)
		printf("system error!");
}
