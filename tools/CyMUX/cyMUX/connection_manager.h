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
 * \file connection_manager.h
 *
 * \brief Info related to handling and processing new client / responder socket
 *        connection requests
 *
 */



#ifndef connection_manager_H
#define connection_manager_H


#include "IPMB.h"


extern void start_connection_manager(void);
extern void stop_connection_manager(void);
extern void process_new_connections(int server_sock);
extern int post_responderQ(Message_t const * const m) __nonnull((1));
extern int send_response(int resource, Message_t const* const msg) __nonnull((2));


#include "msgq.h"
extern MsgQ_t gClientQ;
#define MAX_SOCKET_THREADS  5      // Any more than this and the i2c is too chatty


typedef enum {
    CT_CLIENT = 0,
    CT_RESPONDER,
    CT_INVALID
} connection_t;


#endif /* connection_manager_H */
