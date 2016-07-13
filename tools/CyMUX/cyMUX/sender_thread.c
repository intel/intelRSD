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
 * \file sender_thread.c
 *
 * \brief The main thread and handles all outbound messages on the i2c
 *
 */


#include "sender_thread.h"
#include "worker_thread.h"
#include "cy_i2c.h"
#include "configs.h"
#include "debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>


static pthread_t thread_id;
MsgQ_t           gSenderQ;


static void* sender_thread(void* pArg);
static int post_error(Message_t const* const m, CY_RETURN_STATUS cy_cc);


/*!
 * Initialize the driver for and spin up the thread that will be responsible to
 * sending all outbound i2c messages
 *
 * \brief Initialize sender thread and its resources
 *
 * \author bjkurlex (11/3/15)
 *
 */
void start_i2c_sender(void)
{
    int retval;


    init_i2c();
    retval = msgq_init(&gSenderQ, SENDER_QUEUE_SIZ);
    if (0 != retval) {
        debug(LOG_EMERG, "Unable to initialize sender message queue: %s\n",
              strerror(retval));
        exit(1);
    }

    if (pthread_create(&thread_id, NULL, sender_thread, NULL) < 0) {
        /* Unexplained error, log it and move on */
        debug(LOG_EMERG, "failed to start new sender thread connection: %s\n",
              strerror(errno));
        exit(1);
    }
    debug(LOG_NOTICE, "Started sender thread %ld\n", thread_id);
}


/*!
 * Perform an orderly shutdown of the i2c sending thread and it's resources
 *
 * \author bjkurlex (11/3/15)
 *
 */
void stop_i2c_sender(void)
{
    int   retval;
    void* ret;

    if (thread_id) {
        debug(LOG_NOTICE, "Canceling sender thread %ld\n", thread_id);
        retval = pthread_cancel(thread_id);

        debug(LOG_INFO, "Joining thread %ld (cancel returned %d)\n", thread_id, retval);
        retval = pthread_join(thread_id, &ret);

        debug(LOG_INFO, "Sanity check: thread %ld (join return %d:%p -- should be -1 if cancelled)\n",
                                   thread_id, retval, ret);

        if (PTHREAD_CANCELED != ret) debug(LOG_CRIT, "Failed to cancel i2c sender thread: %s\n", strerror(errno));
    }

    retval = msgq_destroy(&gSenderQ);
    if (0 != retval) {
        debug(LOG_EMERG, "Unable to destroy sender message queue: %s\n",
              strerror(retval));
        exit(1);
    }
}


/*!
 * i2c sending thread.  Opens device for any outbound messages.  Reads messages
 * in the sender message queue for submission and writes them to the i2c bus
 *
 * \author bjkurlex (11/12/15)
 *
 * \brief Thread for sending IPMB messages across the i2c.
 *
 * \param pArg pthread argument -- not used
 *
 * \return void* return value of routine -- not used
 *
 */
static void* sender_thread(void __attribute__((__unused__))* pArg)
{
    CY_HANDLE          handle;
    CY_RETURN_STATUS   retval;
    CY_I2C_DATA_CONFIG data_config;
    CY_DATA_BUFFER     write_buff;
    Message_t          msg;
    int                rc;


    debug(LOG_INFO, "Master at %d:%d\n",
          gCy_bus[I2C_MASTER].device, gCy_bus[I2C_MASTER].interface);

    retval = CyOpen(gCy_bus[I2C_MASTER].device, gCy_bus[I2C_MASTER].interface, &handle);
    if (CY_SUCCESS == retval) {

        //reset xmt module to clear out error, if any
        CyI2cReset(handle, WRITE_MODE);
        debug(LOG_INFO, "i2c sender thread ready\n");

        while (gRunning) {
            rc = msgq_get(&gSenderQ, &msg);
            msgq_debug(&gSenderQ, "gSenderQ");
            if (0 != rc) {
                debug(LOG_EMERG, "Unable to get message from sender message queue: %s\n",
                      strerror(retval));
                exit(1);
            }

            data_config.slaveAddress = msg.DstAddr >> 1;
            data_config.isStopBit = true;
            data_config.isNakBit = false;

            write_buff.buffer = msg.Data;
            write_buff.length = msg.Length;
            write_buff.transferCount = 0;

            message_dump("sender_thread received message to write", &msg);

            retval = CyI2cWrite(handle, &data_config, &write_buff, 2 * MILLISEC);
            if (retval != CY_SUCCESS) {
                // Need to add code to send the fact it NAK'd back to the caller
                debug(LOG_WARNING, "Error: CyI2cWrite failed<%d>: %s\n", retval, cy_strerror(retval));
                CyI2cReset(handle, WRITE_MODE);
                if (valid_request_msg(&msg)) {
                    (void)post_error(&msg, retval);
                    // Ignore failure
                }
            }
            else {
                debug(LOG_INFO, "Wrote packet to i2c (size %d, wrote %d)\n",
                      write_buff.length, write_buff.transferCount);
                if (gSendDelay > 0)
                    usleep(gSendDelay * MILLISEC);
            }
        }
        CyClose(handle);
    }
    else {
        debug(LOG_EMERG, "Open as master i2c failed (is something else partially bound to the part?):\n%s\n", cy_strerror(retval));
        exit(1);
    }


    // We are going to self destruct, so detach so no resources are held
    // (no reaper/join needed)
    pthread_detach(pthread_self());
    pthread_exit(NULL);
}


/*!
 * When an error is encountered writing the message to the i2c, try to return a
 * packet to the requestor that make some sense given the error.
 *
 * \author bjkurlex (11/18/15)
 *
 * \param m The original message tha was attempted to be sent (used to craft a response)
 * \param cy_cc The error code returned when the write failed
 *
 * \return int any errno value determined implicitly or explicitly
 *
 */
static int post_error(Message_t const* const m, CY_RETURN_STATUS cy_cc)
{
    Message_t   m_rsp;
    uint8_t     cc;
    int         retval;

    switch (cy_cc) {
    case CY_ERROR_I2C_NAK_ERROR:
        cc = CC_DEST_UNAVAILABLE;       // Destination unavailable
        break;

    case CY_ERROR_I2C_BUS_BUSY:
    case CY_ERROR_I2C_DEVICE_BUSY:
        cc = CC_NODE_BUSY;              // Node busy
        break;

    case CY_ERROR_IO_TIMEOUT:
        cc = CC_TIMEOUT;                // Time out
        break;

    case CY_ERROR_REQUEST_FAILED:
        cc = CC_COULD_NOT_PROVIDE_RESP; // Could not provide response
        break;

    case CY_ERROR_ALLOCATION_FAILED:
        cc = CC_OUT_OF_SPACE;           // Out of space
        break;

    default:
        cc = CC_UNSPECIFIED_ERR;        // Unspecified error
        break;
    }


    if (0 != craft_error_response(m, &m_rsp, cc)) {
        debug(LOG_WARNING, "Can't recraft message as response\n");
        return EINVAL;
    }

    message_dump("%ld: Posting message to client response queue", &m_rsp, pthread_self());
    retval = msgq_post(&gWorkerQ, &m_rsp);
    msgq_debug(&gWorkerQ, "gWorkerQ");
    return retval;
}
