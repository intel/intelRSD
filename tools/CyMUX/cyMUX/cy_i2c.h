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
 * \file cy_i2c.h
 *
 * \brief Common defines, structures, and declarations for accessing the Cypress
 *        i2c part via the Cypress library
 *
 */


#ifndef cy_i2c_H
#define cy_i2c_H


#include <stdbool.h>            // Needed before CyUSBSerial.h or throws errors
#include "CyUSBSerial.h"


#define I2C_MASTER    0         // SCB 0 = master. this must match with the
                                // configuration in CY7C65215
#define READ_MODE     0

#define I2C_SLAVE     1         // SCB 1 = slave
#define WRITE_MODE    1

#define CY_BUS_MAX    2         // There are only two i2c buses for the CY7C65215


typedef struct {
    UINT8 device;
    UINT8 interface;
} I2cInfo_t;


extern I2cInfo_t gCy_bus[CY_BUS_MAX];
extern bool      gCy_initialized;


extern void init_i2c(void);
extern void cleanup_i2c(void);
extern const char const* cy_strerror(CY_RETURN_STATUS errnum);


#endif /* cy_i2c_H */
