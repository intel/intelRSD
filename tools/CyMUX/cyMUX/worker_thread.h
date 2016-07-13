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
 * \file worker_thread.h
 *
 * \brief Related info to the thread managing IPMB messages to be dispatched
 *        between the i2c and the respective requestor/responder
 *
 */



#ifndef worker_thread_H
#define worker_thread_H


#include "msgq.h"


extern MsgQ_t   gWorkerQ;

extern void start_worker(void);
extern void stop_worker(void);

#define WORKER_QUEUE_SIZ    5
#define PING_RESOURCE       -2      // resource of message to be dropped

#endif /* worker_thread_H */
