/*!
 * \copyright
 * Copyright (c) 2015, 2016 Intel Corporation. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * \file msgq.h
 *
 * \brief Queue definition for messages queues used between threads
 *
 * \note Should migrate to boost libraries when moved to C++
 *
 */



#ifndef msgq_H
#define msgq_H


#include "IPMB.h"
#include <pthread.h>
#include <sys/cdefs.h>


// Queue definition
typedef struct msgq {
    pthread_mutex_t     lock;
    pthread_cond_t      queue_ready;
    pthread_cond_t      data_ready;
    size_t              size;
    size_t              head;
    size_t              tail;
    Message_t*          queue;
} MsgQ_t;


extern int msgq_init(MsgQ_t* const q, const size_t slots) __nonnull((1));
extern int msgq_destroy(MsgQ_t* const q) __nonnull((1));
extern int msgq_get(MsgQ_t* const q, Message_t* const m) __nonnull((1, 2));
extern int msgq_post(MsgQ_t* const q, Message_t const* const m) __nonnull((1, 2));
extern int msgq_flush(MsgQ_t* const q);
extern void msgq_debug(MsgQ_t* const q, const char* const name) __nonnull((1, 2));


#endif /* msgq_H */
