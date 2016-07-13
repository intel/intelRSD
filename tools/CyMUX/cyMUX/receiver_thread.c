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
 * \file receiver_thread.c
 *
 * \brief The main thread that recieves and handles all inbound messages on the i2c
 *
 */


#include "receiver_thread.h"
#include "worker_thread.h"
#include "configs.h"
#include "cy_i2c.h"
#include "debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>


static pthread_t thread_id;


static void* receiver_thread(void* pArg);


/*!
 * Initialize the driver for and spin up the thread that will be responsible to
 * listening and receiving all inbound i2c messages
 *
 * \brief Initialize receiver thread and its resources
 *
 * \author bjkurlex (11/3/15)
 *
 */
void start_i2c_receiver(void)
{
    init_i2c();
    if (pthread_create(&thread_id, NULL, receiver_thread, NULL) < 0) {
        // Unexplained error, log it and move on
        debug(LOG_EMERG, "failed to start new receiver thread connection: %s\n",
                strerror(errno));
    }

    debug(LOG_NOTICE, "Started receiver thread %ld\n", thread_id);
}


/*!
 * Perform an orderly shutdown of the i2c receiving thread and it's resources
 *
 * \author bjkurlex (11/3/15)
 *
 */
void stop_i2c_receiver(void)
{
    int   retval;
    void* ret;


    if (thread_id) {
        debug(LOG_NOTICE, "Canceling receiver thread %ld\n", thread_id);
        retval = pthread_cancel(thread_id);

        debug(LOG_INFO, "Joining thread %ld (cancel returned %d)\n", thread_id, retval);
        retval = pthread_join(thread_id, &ret);

        debug(LOG_INFO, "Sanity check: thread %ld (join return %d:%p -- should be -1 if cancelled)\n",
              thread_id, retval, ret);

        if (PTHREAD_CANCELED != ret)
            debug(LOG_CRIT, "Failed to cancel i2c receiver thread: %s\n", strerror(errno));
    }
}


/*!
 * i2c listner thread.  Opens device and receives any inbound messages.  Posts
 * the messages to the worker thread message queue for further processing.
 *
 * \author bjkurlex (11/12/15)
 *
 * \brief Thread for receiving IPMB messages that come across the i2c.
 *
 * \param pArg pthread argument -- not used
 *
 * \return void* return value of routine -- not used
 *
 */
static void* receiver_thread(void __attribute__((__unused__))* pArg)
{
    CY_HANDLE          handle      = NULL;
    CY_RETURN_STATUS   retval      = CY_SUCCESS;
    CY_I2C_DATA_CONFIG data_config;
    CY_DATA_BUFFER     read_buff;
    Message_t          msg;
    uint8_t            raw_buff[MAX_IPMB_PKT_SIZE];


    bzero(&msg, sizeof(msg));
    bzero(raw_buff, sizeof(raw_buff));
    read_buff.buffer = raw_buff;
    read_buff.transferCount = 0;

    debug(LOG_INFO, "Slave at %d:%d\n",
          gCy_bus[I2C_SLAVE].device, gCy_bus[I2C_SLAVE].interface);

    retval = CyOpen(gCy_bus[I2C_SLAVE].device, gCy_bus[I2C_SLAVE].interface, &handle);

    if (CY_SUCCESS == retval) {

        // reset xmt module to clear out error, if any
        CyI2cReset(handle, READ_MODE);
        debug(LOG_INFO, "i2c receiver thread ready\n");

        data_config.slaveAddress = 0x00;    // slave address (is a random number)
                                            // used in master read command, not
                                            // used in slave mode
        data_config.isStopBit = true;
        data_config.isNakBit = false;

        read_buff.buffer = msg.Data;
        read_buff.length = sizeof(raw_buff);
        read_buff.transferCount = 0;

        msg.DstAddr = I2C_ADDR << 1;        // If we received it, it was to our address
        msg.client_resource = -1;           // Flag as from the I2C

        while (gRunning) {
            retval = CyI2cRead(handle, &data_config, &read_buff, 60 * MILLISEC);
            switch (retval) {
            case CY_ERROR_I2C_STOP_BIT_SET:
                // read a message

                msg.Length = read_buff.transferCount;
                // Post to message handler queue
                message_dump("Packet read from i2c (%d bytes read)",
                             &msg, read_buff.transferCount);

                if (0 == read_buff.transferCount) {
                    debug(LOG_ALERT, "Dropping zero length packet read from i2c");
                    break;
                }

                retval = msgq_post(&gWorkerQ, &msg);
                if (0 != retval) {
                    debug(LOG_EMERG, "(ignoring) msgq_post error %s\n", strerror(retval));
                }
                msgq_debug(&gWorkerQ, "gWorkerQ");
                break;

            default:
                // Catastrophic error
                debug(LOG_EMERG, "Error reading i2c: %s\n", cy_strerror(retval));
                break;

            case CY_ERROR_IO_TIMEOUT:
            case CY_ERROR_I2C_DEVICE_BUSY:
                CyI2cReset(handle, READ_MODE);
                break;

            }
        }
        CyClose(handle);
    }
    else {
        // Dump to stderr as we shouldn't have closed the console just yet
        debug(LOG_EMERG, "Open as slave i2c failed (is something else partially bound to the part?):\n%s\n", cy_strerror(retval));
        exit(1);
    }


    // We are going to self destruct, so detach so no resources are held
    // (no reaper/join needed)
    pthread_detach(pthread_self());
    pthread_exit(NULL);
}
