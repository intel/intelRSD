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


//#include "buffer_cache.h"
//#include "liblog/liblog.h"
#if 0
buffer_cache_t *bc_create(int len, int number)
{
    buffer_cache_t *cache = (buffer_cache_t *)malloc(sizeof(buffer_cache_t));
    bzero(cache, sizeof(buffer_cache_t));

    if (pthread_mutex_init(&cache->mutex, NULL) != 0)
        return NULL;

    cache->len = len;
    cache->nr_max = number;
    return cache;
}

void *bc_alloc(buffer_cache_t *bc)
{
    pthread_mutex_lock(&bc->mutex);

    buffer_t *buf_list = NULL;
    if (bc->buf_list != NULL) {
        buf_list = bc->buf_list;
        bc->buf_list = buf_list->next;
        bc->nr--;
    } else {
        buf_list = (buffer_t *)malloc(sizeof(buffer_t));
        assert(buf_list);
        bzero(buf_list, sizeof(buffer_t));
        buf_list->buf = malloc(bc->len + sizeof(long long));
        (*(long long *)buf_list->buf) = (long long)buf_list;
        log_dbg("buf_list=%p, buf_list->buf=%p\n", buf_list, buf_list->buf);
    }
    pthread_mutex_unlock(&bc->mutex);
    return buf_list->buf + sizeof(long long);
}

void bc_free(buffer_cache_t *bc, void *buf)
{
    long long *ptr = buf;
    buffer_t *buf_list;
    pthread_mutex_lock(&bc->mutex);

    ptr -= 1;
    buf_list = (buffer_t *)*ptr;
    log_dbg("buf_list=%p, buf_list->buf=%p\n", buf_list, buf_list->buf);

    if (bc->nr >= bc->nr_max) {
        free(buf_list->buf);
        free(buf_list);
    } else {
        buf_list->next = bc->buf_list;
        bc->buf_list = buf_list;
        bc->nr++;
    }

    pthread_mutex_unlock(&bc->mutex);
}
#endif
