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
 * @file pca95xx_errors.c
 *
 * @brief PCA95XX errors implementation
 * */

#include "pca95xx/pca95xx_errors.h"

/*! Array size, number of elements */
#define MAX_ARRAY(array)            (sizeof(array)/sizeof(array[0]))

/*! Error key:value map */
static const struct _error {
    enum pca95xx_status code;
    const char* str;
}g_errors_table[] = {
    {PCA95XX_ERROR,                     "Unknown error"},
    {PCA95XX_ERROR_NULL,                "Null pointer"},
    {PCA95XX_ERROR_I2C_OPEN,            "I2C peripheral cannot be open"},
    {PCA95XX_ERROR_I2C_CLOSE,           "I2C peripherel cannot be close"},
    {PCA95XX_ERROR_I2C_BUS_RANGE,       "I2C bus number out of range"},
    {PCA95XX_ERROR_I2C_ADDRESS,         "I2C slave address is invalid"},
    {PCA95XX_ERROR_I2C_WRITE_READ,      "I2C write/read error"},
    {PCA95XX_ERROR_IO_BANK_RANGE,       "IO bank out of range"},
    {PCA95XX_ERROR_UNKNOWN_MODEL,       "Unknown model"},
    {PCA95XX_ERROR_UNKNOWN_REGISTER,    "Unknown register"},
};

const char* pca95xx_get_error_string(int error_code) {
    for (unsigned int i=0; i<MAX_ARRAY(g_errors_table); ++i) {
        if (g_errors_table[i].code == error_code) {
            return g_errors_table[i].str;
        }
    }

    return "Unknown error";
}
