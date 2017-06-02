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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libsecurec/safe_lib.h"


int main(void)
{
	int rc = 0;
	char platform[32];

	fprintf(stderr, "Please select your platform (BDC-A or BDC-R): ");
	fgets(platform, 32, stdin);

    if (strncmp(platform, "BDC-A", strnlen_s("BDC-A", RSIZE_MAX_STR)) == 0  || strncmp(platform, "BDC-R", strnlen_s("BDC-R", RSIZE_MAX_STR)) == 0) {
        fprintf(stderr, "Do init for platform %s\n", platform);
        if (strncmp(platform, "BDC-R", strnlen_s("BDC-R", RSIZE_MAX_STR)) == 0) {
            rc = system("sed -i 's/BDC-A/BDC-R/g' /etc/rmm/rmm.cfg");
        }
    } else {
        fprintf(stderr, "Input wrong platform, so do init default for platform BDC-A ...\n");
    }

	return rc;
}
