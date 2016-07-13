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
 * \file debug.c
 *
 * \brief Generic routines dealing with debugging / diagnostics
 *
 */


#include "debug.h"
#include "msgq.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>


// Serialize debug messages to the console
static pthread_mutex_t gDebugLock;


// Verbosity of info/debug to dump
int                    gVerbosity = 0;


static void handle_signal(int signal);


/*!
 * Debug routine to dump a message packet along with a message
 * I kept copying and pasting this pattern that I finally made it its own routine
 *
 * \author bjkurlex (11/18/15)
 *
 * \param fmt printf style format string/message
 * \param m Message packet to dump
 * \param ... optional parameters to pass for use with fmt
 *
 */
void message_dump(char const* const fmt, Message_t const* const m, ...)
{
    int     retval;
    va_list ap;


    if (NULL == fmt || NULL == m) {
        // If parameters aren't valid, we've got nothing to dump
        return;
    }

    if (gVerbosity >= LOG_DEBUG) {
        retval = pthread_mutex_lock(&gDebugLock);
        if (0 != retval) {
            // "shouldn't" get here
            fprintf(stderr, "Unable to lock debug mutex: %s\n",
                    strerror(retval));
            exit(1);
        }

        va_start(ap, m);
        vprintf(fmt, ap);
        va_end(ap);
        printf("\n");
        printf("Resource: %d\n", m->client_resource);
        printf("DstAddr: %02x\n", m->DstAddr);
        for (size_t i = 0; i < m->Length; i++) {
            if (0 == (i % 16)) printf("\n%04ld:", i);
            printf(" %02x", m->Data[i]);
        }
        printf("\n\n");

        retval = pthread_mutex_unlock(&gDebugLock);
        if (0 != retval) {
            // "shouldn't" get here
            fprintf(stderr, "Unable to unlock debug mutex: %s\n",
                    strerror(retval));
            exit(1);
        }
    }
}


/*!
 * Debug routine to dump a buffer along with a message
 * I kept copying and pasting this pattern that I finally made it its own routine
 *
 * \author bjkurlex (11/18/15)
 *
 * \param fmt printf style format string/message
 * \param b Message packet to dump
 * \param l length of the buffer to dump
 * \param ... optional parameters to pass for use with fmt
 *
 */
void buffer_dump(char const* const fmt, uint8_t const* const b, ssize_t l, ...)
{
    int     retval;
    va_list ap;


    if (NULL == fmt || NULL == b) {
        // If parameters aren't valid, we've got nothing to dump
        return;
    }

    if (gVerbosity >= LOG_DEBUG) {
        retval = pthread_mutex_lock(&gDebugLock);
        if (0 != retval) {
            // "shouldn't" get here
            fprintf(stderr, "Unable to lock debug mutex: %s\n",
                    strerror(retval));
            exit(1);
        }

        va_start(ap, l);
        vprintf(fmt, ap);
        va_end(ap);
        for (ssize_t i = 0; i < l; i++) {
            if (0 == (i % 16)) printf("\n%04ld:", i);
            printf(" %02x", b[i]);
        }
        printf("\n\n");

        retval = pthread_mutex_unlock(&gDebugLock);
        if (0 != retval) {
            // "shouldn't" get here
            fprintf(stderr, "Unable to unlock debug mutex: %s\n",
                    strerror(retval));
            exit(1);
        }
    }
}


/*!
 * Debug routine to dump message to the console
 *
 * \author bjkurlex (11/19/15)
 *
 * \param fmt printf style format string/message
 * \param ... optional parameters to pass for use with fmt
 *
 */
void debug(int verbosity, char const* const fmt, ...)
{
    int     retval;
    va_list ap;


    if (NULL == fmt) {
        // If parameters aren't valid, we've got nothing to dump
        return;
    }

    if (verbosity <= gVerbosity) {
        retval = pthread_mutex_lock(&gDebugLock);
        if (0 != retval) {
            // "shouldn't" get here
            fprintf(stderr, "Unable to lock debug mutex: %s\n",
                    strerror(retval));
            exit(1);
        }

        printf("%ld: ", time(NULL));
        va_start(ap, fmt);
        vprintf(fmt, ap);
        va_end(ap);

        retval = pthread_mutex_unlock(&gDebugLock);
        if (0 != retval) {
            // "shouldn't" get here
            fprintf(stderr, "Unable to unlock debug mutex: %s\n",
                    strerror(retval));
            exit(1);
        }
    }
}


/*!
 * Routine to initialize the debug subsystem
 *
 * \author bjkurlex (11/19/15)
 *
 */
void init_diag(void)
{
    int retval;
    struct sigaction sa;

    retval = pthread_mutex_init(&gDebugLock, NULL);
    if (0 != retval) {
        // "shouldn't" get here
        fprintf(stderr, "Unable to initialize debug mutex: %s\n",
                strerror(retval));
        exit(1);
    }

    // Set up SIGUSR1/USR2 to handle log level
    sa.sa_handler = &handle_signal;
    sa.sa_flags = SA_RESTART;       // restart system calls, if possible
    sigfillset(&sa.sa_mask);        // BLock every signal during the handler

    if (-1 == sigaction(SIGUSR1, &sa, NULL)) {
        fprintf(stderr, "Unable to handle SIGUSR1: %s\n",
                strerror(retval));
        exit(1);
    }

    if (-1 == sigaction(SIGUSR2, &sa, NULL)) {
        fprintf(stderr, "Unable to handle SIGUSR1: %s\n",
                strerror(retval));
        exit(1);
    }
}


/*!
 * Routine to deinitialize the debug subsystem
 *
 * \author bjkurlex (11/19/15)
 *
 */
void cleanup_diag(void)
{
    int retval;

    retval = pthread_mutex_destroy(&gDebugLock);
    if (0 != retval) {
        // "shouldn't" get here
        fprintf(stderr, "Unable to destroy debug mutex: %s\n",
                strerror(retval));
        exit(1);
    }
}


/*!
 * Debug routine to dump message queue pointers
 *
 * \param q Queue to dump
 * \param name string name of queue for the debug output
 *
 */
void msgq_debug(MsgQ_t* const q, const char* const name)
{
    int retval;

    assert(NULL != name);
    assert(NULL != q->queue);

    retval = pthread_mutex_lock(&q->lock);
    assert(0 == retval);

    debug(LOG_INFO, "%ld: %s size: %ld; head: %ld; tail: %ld; used: %ld\n",
           time(NULL), name, q->size, q->head, q->tail,
           (q->size + q->head - q->tail) % q->size);

    retval = pthread_mutex_unlock(&q->lock);
    assert(0 == retval);
}


/*!
 * Signal handler to allow dynamic changing of the log level while running
 *
 * \param signal signal received
 *
 */
static void handle_signal(int signal)
{
    if (SIGUSR1 == signal) {
        gVerbosity++;
    }
    else if (SIGUSR2 == signal && gVerbosity > 0) {
        gVerbosity--;
    }
}
