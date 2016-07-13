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
 * \file worker_thread.c
 *
 * \brief The main thread that handles dispatching of IPMB messages between the
 *        i2c threads and the client/responder threads
 *
 */


#include "worker_thread.h"
#include "sender_thread.h"
#include "connection_manager.h"
#include "configs.h"
#include "ll.h"
#include "debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <assert.h>


MsgQ_t           gWorkerQ;
static pthread_t thread_id;


static void* worker_thread(void* pArg);


/*!
 * Initialize and spin up the thread that will be responsible for dispatching
 * IPMB messages between the i2c threads and the client/responder threads.
 *
 * \brief initialize worker thread and its resources
 *
 * \author bjkurlex (11/3/15)
 *
 */
void start_worker(void)
{
    int retval;


    retval = msgq_init(&gWorkerQ, WORKER_QUEUE_SIZ);
    assert(0 == retval);

    retval = ll_init();
    assert(0 == retval);

    if (pthread_create(&thread_id, NULL, worker_thread, NULL) < 0) {
        // Unexplained error, log it and move on
        fprintf(stderr, "failed to start new worker thread connection: %s\n",
              strerror(errno));
        exit(1);
    }
    debug(LOG_NOTICE, "Started worker thread %ld\n", thread_id);
}


/*!
 * Perform an orderly shutdown of the worker thread and it's resources
 *
 * \author bjkurlex (11/3/15)
 *
 */
void stop_worker(void)
{
    int   retval;
    void* ret;

    if (thread_id) {
        debug(LOG_NOTICE, "Canceling worker thread %ld\n", thread_id);

        retval = pthread_cancel(thread_id);
        debug(LOG_INFO, "Joining thread %ld (cancel returned %d)\n",
              thread_id, retval);

        retval = pthread_join(thread_id, &ret);
        debug(LOG_INFO, "Sanity check: thread %ld (join return %d:%p -- should be -1 if cancelled)\n",
              thread_id, retval, ret);

        if (PTHREAD_CANCELED != ret)
            debug(LOG_CRIT, "Failed to cancel i2c worker thread: %s\n", strerror(errno));
    }

    retval = msgq_destroy(&gWorkerQ);
    assert(0 == retval);

    retval = ll_destroy();
    assert(0 == retval);
}


/*!
 * Worker thread responsible for dispatching messages.  Redirects to responder,
 * client, or the sender_thread
 *
 * \author bjkurlex (11/3/15)
 *
 * \param pArg pthread argument -- not used
 *
 * \return void* return value of routine -- not used
 *
 */
static void* worker_thread(void __attribute__((__unused__))* pArg)
{
    int         retval;
    Message_t   msg;
    pthread_t   id = pthread_self();

    while (gRunning) {
        retval = msgq_get(&gWorkerQ, &msg);
        assert(0 == retval);
        msgq_debug(&gWorkerQ, "gWorkerQ");
        message_dump("Worker thread received message:", &msg);

        if (msg.Length >= (sizeof(IpmbReq_t) + 1UL)) {
            if ((I2C_ADDR << 1) == msg.DstAddr) {
                // Inbound message
                if (0 == (msg.Data[0] & (1 << 2))) {
                    // Direct BMC request
                    debug(LOG_INFO, "worker thread redirecting to responder Q\n");
                    if (PING_RESOURCE == msg.client_resource) {
                        // We want to drop the response packet, so leave the bride at the alter
                        retval = post_responderQ(&msg);
                        assert(0 == retval);
                    }
                    else {
                        debug(LOG_INFO, "worker thread saving groom\n");

                        if (0 == ll_post(&msg)) {   // Save groom (request) to wait for bride (response)
                            // Only if it posted successfully (possibly a bad packet, so there's nothing we can do)
                            retval = post_responderQ(&msg);
                            assert(0 == retval);
                        }
                    }
                }
                else {
                    // Response packet
                    message_dump("%ld: posting message to clientQ:", &msg, id);

                    retval = msgq_post(&gClientQ, &msg);
                    assert(0 == retval);
                }
            }
            else {
                // Outbound message
                debug(LOG_INFO, "worker thread received an outbound request packet\n");
                if (0 == (msg.Data[0] & (1 << 2))) {
                    // Request
                    debug(LOG_INFO, "worker thread saving groom\n");
                    (void)ll_post(&msg);   // Save groom (request) to wait for bride (response)
                }
                else {
                    int          resource;

                    // Marry the bride(response) to the waiting groom(request)
                    retval = ll_get(&resource, &msg, true);
                    assert(-1 != retval);
                }

                if (PING_RESOURCE != msg.client_resource) {
                    debug(LOG_INFO, "worker thread posting message to sender Q\n");
                    retval = msgq_post(&gSenderQ, &msg);        // Send the request off
                    assert(0 == retval);
                    msgq_debug(&gSenderQ, "gSenderQ");
                }
                // drop ping response packets
            }
        }
    }

    // We are going to self destruct, so detach so no resources are held
    // (no reaper/join needed)
    pthread_detach(pthread_self());
    pthread_exit(NULL);
}
