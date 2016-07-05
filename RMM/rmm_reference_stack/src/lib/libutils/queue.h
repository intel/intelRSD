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


#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <pthread.h>
#include <strings.h>
#include <stdlib.h>
#include <assert.h>

#define MSG_POOL_MAX	(128)

typedef struct msg_s {
    void *data;
    int type;
    int len;		
} msg_t;

typedef struct msg_list_s {
    msg_t msg;
    struct msg_list_s *next;
} msg_list_t;

typedef struct queue_s {
    pthread_mutex_t mutex;
    pthread_cond_t cond;

    msg_list_t *first;
    msg_list_t *last;
    int msg_nr;

    msg_list_t *msg_pool;
    int msg_pool_nr;	
} queue_t;

queue_t *queue_init(void);

void queue_add(queue_t *q, void *data, int len, int type);

void queue_get(queue_t *q, msg_t *msg);

#endif
