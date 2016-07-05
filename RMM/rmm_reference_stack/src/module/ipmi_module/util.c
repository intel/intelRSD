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
#include <string.h>

#include "ipmi.h"
#include "ipmi_log.h"

/**
  *  @brief caculate IPMI msg checksum
  *
  *  @param[in/out] data IPMI data
  *  @param[in] size IPMI data length
  *  @return
  */
unsigned char ipmi_csum(unsigned char *data, int size)
{
	unsigned char csum = 0;

	for (; size > 0; size--, data++)
		csum += *data;

	return -csum;
}

/**
  *  @brief create memory free list
  *
  *  @param[in] num memory number
  *  @param[in] size each memory block size
  *  @return
  */
void *mem_freelist_create(int num, size_t size)
{
	/*
	 * Member 'next' in freelist structure definition must be first,
	 * other members following.
	 */
	struct mem_freelist {
		struct mem_freelist *next;
	};

	int i;
	void *result, *pmem;
	struct mem_freelist *list;

#ifdef DEBUG_IPMI
	static size_t total;

	total += (num * size);
	IPMI_LOG_DEBUG("Alloc freelist memmory size : %llu\n", (unsigned long long)(num * size));
	IPMI_LOG_DEBUG("Total used for freelist : %llu KB\n",  (unsigned long long)(total/1024));
#endif

	if ((result = malloc(num * size)) == NULL)
		return NULL;

	for (i = 0, pmem = result; i < (num - 1); i++) {
		list  = pmem;
		pmem += size;
		list->next = pmem;
	}

	list = pmem;
	list->next = NULL;

	return result;
}

