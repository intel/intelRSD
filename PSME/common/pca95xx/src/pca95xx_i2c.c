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
 * @file pca95xx_i2c.c
 *
 * @brief PCA95XX I2C implementation
 * */

#include "pca95xx/pca95xx.h"
#include "pca95xx/pca95xx_i2c.h"

#include <linux/types.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

/*! Maximum characters for device filename, linux */
#define DEV_FILENAME_MAX                12

/*! Max I2C bus for PCA95xx */
#define PCA95XX_I2C_MAX_BUS_NUMBER      100

int pca95xx_i2c_write(unsigned int bus_number,
        unsigned int slave_address,
        unsigned int command,
        unsigned int value) {

    if (!(bus_number < PCA95XX_I2C_MAX_BUS_NUMBER)) {
        return PCA95XX_ERROR_I2C_BUS_RANGE;
    }

    int status = 0;
    char filename[DEV_FILENAME_MAX] = "";
    snprintf(filename, DEV_FILENAME_MAX-1, "/dev/i2c-%d", bus_number);

    union i2c_smbus_data i2c_data;
    struct i2c_smbus_ioctl_data i2c_ioctl;

    i2c_data.byte = (__u8)value;

    i2c_ioctl.read_write = I2C_SMBUS_WRITE;
    i2c_ioctl.command = (__u8)command;
    i2c_ioctl.size = I2C_SMBUS_BYTE_DATA;
    i2c_ioctl.data = &i2c_data;

    int file = open(filename, O_RDWR);
    if (file < 0) {
        printf("%s\n", strerror(errno));
        return PCA95XX_ERROR_I2C_OPEN;
    }

    status = ioctl(file, I2C_SLAVE, slave_address);
    if (status < 0) {
        printf("%s\n", strerror(errno));
        close(file);
        return PCA95XX_ERROR_I2C_ADDRESS;
    }

    status = ioctl(file, I2C_SMBUS, &i2c_ioctl);
    if (status < 0) {
        printf("%s\n", strerror(errno));
        close(file);
        return PCA95XX_ERROR_I2C_WRITE_READ;
    }

    status = close(file);
    if (status < 0) {
        printf("%s\n", strerror(errno));
        return PCA95XX_ERROR_I2C_CLOSE;
    }

    return PCA95XX_SUCCESS;
}

int pca95xx_i2c_read(unsigned int bus_number,
        unsigned int slave_address,
        unsigned int command) {

    if (!(bus_number < PCA95XX_I2C_MAX_BUS_NUMBER)) {
        return PCA95XX_ERROR_I2C_BUS_RANGE;
    }

    int status = 0;
    char filename[DEV_FILENAME_MAX] = "";
    snprintf(filename, DEV_FILENAME_MAX-1, "/dev/i2c-%d", bus_number);

    union i2c_smbus_data i2c_data;
    struct i2c_smbus_ioctl_data i2c_ioctl;

    i2c_data.byte = 0;

    i2c_ioctl.read_write = I2C_SMBUS_READ;
    i2c_ioctl.command = (__u8)command;
    i2c_ioctl.size = I2C_SMBUS_BYTE_DATA;
    i2c_ioctl.data = &i2c_data;

    int file = open(filename, O_RDWR);
    if (file < 0) {
        printf("%s\n", strerror(errno));
        return PCA95XX_ERROR_I2C_OPEN;
    }

    status = ioctl(file, I2C_SLAVE, slave_address);
    if (status < 0) {
        printf("%s\n", strerror(errno));
        close(file);
        return PCA95XX_ERROR_I2C_ADDRESS;
    }

    status = ioctl(file, I2C_SMBUS, &i2c_ioctl);
    if (status < 0) {
        printf("%s\n", strerror(errno));
        close(file);
        return PCA95XX_ERROR_I2C_WRITE_READ;
    }

    status = close(file);
    if (status < 0) {
        printf("%s\n", strerror(errno));
        return PCA95XX_ERROR_I2C_CLOSE;
    }

    return i2c_data.byte;
}
