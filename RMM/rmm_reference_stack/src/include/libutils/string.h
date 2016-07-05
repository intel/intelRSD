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


#ifndef __LIBUTILS_STRING_H__
#define __LIBUTILS_STRING_H__

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "libutils/types.h"
#include "libsecurec/safe_lib.h"

static inline char *strncpy_safe(int8 *dest, const int8 *src, size_t buf_len, size_t n)
{	
	if (src == NULL || dest == NULL || n <= 0)
		return NULL;

	assert(n < buf_len);

	strncpy_s(dest, buf_len, src, n);
	//strncpy(dest, src, n);
	
	dest[buf_len - 1] = '\0';

	return dest;
}


#endif
