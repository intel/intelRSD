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
 * \file msgq.c
 *
 * \brief Handle internal message passing between threads
 *
 */


#include "msgq.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>


// shim for calloc/free for testing interception
static void *sys_calloc (size_t __nmemb, size_t __size);
static void sys_free (void *__ptr);


/*!
 * Initialize the message queue and its resources
 *
 * \author bjkurlex (11/9/15)
 *
 * \param q Queue to initialize
 * \param slots Number of entries to allocate for queue
 *
 * \return int any errno value determined implicitly or explicitly
 *
 */
int msgq_init(MsgQ_t* const q, const size_t slots)
{
    int     retval = 0;
    MsgQ_t  old;

    if (NULL == q || slots < 1U) return EINVAL;

    if (NULL != q->queue) return EINVAL;

    old = *q;                       // Save a copy for restoration on failure

    // Error block
    do {
        q->queue = sys_calloc(slots + 1, sizeof(Message_t));
        if (NULL == q->queue) {
            retval = ENOMEM;
            break;
        }

        retval = pthread_mutex_init(&q->lock, NULL);
        if (retval) {
            sys_free(q->queue);     // Restore memory
            break;
        }

        retval = pthread_cond_init(&q->queue_ready, NULL);
        if (retval) {
            sys_free(q->queue);     // Restore memory
            pthread_mutex_destroy(&q->lock);
            break;
        }

        retval = pthread_cond_init(&q->data_ready, NULL);
        if (retval) {
            sys_free(q->queue);     // Restore memory
            pthread_mutex_destroy(&q->lock);
            pthread_cond_destroy(&q->data_ready);
            break;
        }

        q->size = slots + 1U;
        q->head = 0U;
        q->tail = 0U;

    } while (false);

    if (0 != retval) {
        // Error encountered restore block
        *q = old;
    }

    return retval;
}


/*!
 * Tear down the message queue and its resources
 *
 * \author bjkurlex (11/9/15)
 *
 * \param q Queue to destroy
 *
 * \return int any errno value determined implicitly or explicitly
 *
 */
int msgq_destroy(MsgQ_t* const q)
{
    int retval;

    if (NULL == q) return EINVAL;
    if (NULL == q->queue) return EINVAL;

    sys_free(q->queue);
    q->queue = NULL;

    retval = pthread_mutex_destroy(&q->lock);
    if (retval) return retval;

    retval = pthread_cond_destroy(&q->queue_ready);
    if (retval) return retval;

    retval = pthread_cond_destroy(&q->data_ready);
    if (retval) return retval;


    bzero(q, sizeof(struct msgq));

    return retval;
}


/*!
 * Take a message and post it to the requested queue.  Others listening on this
 * queue will be notified of the new message.  Will block if there is no room in
 * the queue until notified there is room available.   The message is copied to
 * the queue so that there is no reliance on the storage of the message passed.
 *
 * \brief Post message to specified queue
 *
 * \author bjkurlex (11/9/15)
 *
 * \param q Queue to post message to
 * \param m The message to post
 *
 * \return int any errno value determined implicitly or explicitly
 *
 */
int msgq_post(MsgQ_t* const q, Message_t const* const m)
{
    int retval;

    if (NULL == q || NULL == m) return EINVAL;
    if (NULL == q->queue) return EINVAL;
    if (q->size < 2) return EINVAL;
    if (0 == m->Length) return EINVAL;
    if (0 == m->DstAddr) return EINVAL;
    if (m->DstAddr & 1) return EINVAL;


    retval = pthread_mutex_lock(&q->lock);
    if (0 != retval) return retval;

    while ((q->head + 1U) % q->size == q->tail) {
        retval = pthread_cond_wait(&q->queue_ready, &q->lock);
        if (0 != retval) {
            if (0 == pthread_mutex_unlock(&q->lock)) {}
            return retval;
        }
    }
    //printf("%ld: Posting Message (q: %p head: %d tail: %d)\n", pthread_self(), (void*)q, q->head, q->tail);

    q->head = (q->head + 1U) % q->size;
    q->queue[q->head] = *m;

    retval = pthread_cond_signal(&q->data_ready);
    if (0 != retval) {
        if (0 == pthread_mutex_unlock(&q->lock)) {}
        return retval;
    }

    return pthread_mutex_unlock(&q->lock);
}


/*!
 * Fetch a message from the requested queue and store it to the address provided.
 * Will block if the queue is empty until a new message is available.
 *
 * \brief Get message from specified queue
 *
 * \author bjkurlex (11/9/15)
 *
 * \param q Queue to get message from
 * \param m Where to store message received
 *
 * \return int any errno value determined implicitly or explicitly
 *
 */
int msgq_get(MsgQ_t* const q, Message_t* const m)
{
    int retval;

    if (NULL == q || NULL == m) return EINVAL;
    if (NULL == q->queue) return EINVAL;
    if (q->size < 2) return EINVAL;

    retval = pthread_mutex_lock(&q->lock);
    if (0 != retval) return retval;

    while (q->head == q->tail) {
        retval = pthread_cond_wait(&q->data_ready, &q->lock);
        if (0 != retval) {
            if (0 == pthread_mutex_unlock(&q->lock)) {}
            return retval;
        }
    }

    q->tail = (q->tail + 1U) % q->size;
    *m = q->queue[q->tail];
    //assert(m->Length);
    //assert(m->DstAddr);
    bzero(&q->queue[q->tail], sizeof(Message_t));


    retval = pthread_cond_signal(&q->queue_ready);
    if (0 != retval) {
        if (0 == pthread_mutex_unlock(&q->lock)) {}
        return retval;
    }

    return pthread_mutex_unlock(&q->lock);
}


/*!
 * Flush ore reset the requested queue
 *
 * \brief Flush queue
 *
 * \param q Queue to flush messages from
 *
 * \return int any errno value determined implicitly or explicitly
 *
 */
int msgq_flush(MsgQ_t* const q)
{
    int retval;

    if (NULL == q) return EINVAL;
    if (NULL == q->queue) return EINVAL;
    if (q->size < 2) return EINVAL;

    retval = pthread_mutex_lock(&q->lock);
    if (0 != retval) return retval;

    q->head = 0;
    q->tail = 0;
    bzero(&q->queue[q->tail], sizeof(Message_t));

    retval = pthread_cond_signal(&q->queue_ready);
    if (0 != retval) {
        if (0 == pthread_mutex_unlock(&q->lock)) {}
        return retval;
    }

    return pthread_mutex_unlock(&q->lock);
}


#ifndef _TAP_SOURCE
/// System shims to use.   testing provides their own routines for simulating
/// errors or confirming proper usage

static void sys_free (void *__ptr)
{
    free(__ptr);
}


static void *sys_calloc (size_t __nmemb, size_t __size)
{
    return calloc(__nmemb, __size);
}
#endif
