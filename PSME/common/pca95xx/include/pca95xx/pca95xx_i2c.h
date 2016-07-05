/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @section DESCRIPTION
 *
 * @file pca95xx/pca95xx_i2c.h
 *
 * @brief PCA95XX I2C interface
 * */

#ifndef PCA95XX_I2C_H
#define PCA95XX_I2C_H

#include "pca95xx/pca95xx.h"

/*!
 * @brief PCA95xx I2C interface for communication
 *
 * This function will write command byte and data byte on I2C bus to
 * IC slave that match address
 *
 * @param[in]   bus_number      Choose I2C bus. 0 means first I2C peripheral
 * @param[in]   slave_address   PCA95xx slave address between 0x00-0x7F
 * @param[in]   command         Command byte for PCA95xx device
 * @param[in]   value           Data byte for PCA95xx device
 * @return      pca95xx_status or other negative error code (example from
 *              third party library)
 * */
int pca95xx_i2c_write(unsigned int bus_number,
        unsigned int slave_address,
        unsigned int command,
        unsigned int value);

/*!
 * @brief PCA95xx I2C interface for communication
 *
 * This function will write command byte on I2C bus to IC slave and read data
 * byte on I2C bus from IC slave that match address
 *
 * @param[in]   bus_number      Choose I2C bus. 0 means first I2C peripheral
 * @param[in]   slave_address   PCA95xx slave address between 0x00-0x7F
 * @param[in]   command         Command byte for PCA95xx device
 * @return      Data byte between 0x00-0xFF otherwise error (negative number)
 * */
int pca95xx_i2c_read(unsigned int bus_number,
        unsigned int slave_address,
        unsigned int command);

#endif /* PCA95XX_I2C_H */
