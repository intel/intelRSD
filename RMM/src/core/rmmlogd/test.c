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


#include <stdlib.h>
#include <stdio.h>

#include "log_manager.h"

int main(int argc, char **argv)
{
	int i = 1;
	char *data;

	log_mgr_init();

	for (i = 1; i < 1200; i++)
		log_mgr_put("memdbd", 1, "call_test", "test log message.........\n");

	data = (char *)malloc(256 * 10);

	log_mgr_get("memdbd", 10, data);
	for (i = 0; i < 10; i++)
		printf("data: %s", data + 256 * i);

	free(data);

	return 0;
}
