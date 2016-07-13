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
 * \file configs.h
 *
 * \brief General configuration both internal and external
 *
 */



#ifndef configs_H
#define configs_H


#include <stdbool.h>


#define LISTENER_PORT   5623        // What port we will take new connections from
#define I2C_ADDR        0x08        // must agree with configuration in SCB 1
#define MILLISEC        1000


// Variables set by the command line
extern int gTimeout;                // Send/Receive timeout to use
extern int gSendDelay;              // How long to delay in ms after sending I2C
extern bool gRunning;               // Flog to let threads know they are still running


#endif /* configs_H */
