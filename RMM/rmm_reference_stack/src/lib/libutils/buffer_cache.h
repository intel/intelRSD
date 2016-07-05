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


#ifndef __BUFFER_CACHE_H__
#define __BUFFER_CACHE_H__

#include <stdlib.h>
#include <strings.h>
#include <assert.h>
#include <pthread.h>

typedef struct buffer_s {
    void *buf;
    struct buffer_s *next;
} buffer_t;

typedef struct buffer_cache_s {
    pthread_mutex_t mutex;
    int len;
    int nr;
    int nr_max;
    buffer_t *buf_list;
} buffer_cache_t;

buffer_cache_t *bc_create(int len, int number);

void *bc_alloc(buffer_cache_t *bc);

void bc_free(buffer_cache_t *bc, void *buf);

#endif
