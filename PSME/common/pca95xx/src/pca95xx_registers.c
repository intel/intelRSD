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
 * @file pca95xx_registers.c
 *
 * @brief PCA95XX registers implementation
 * */

#include "pca95xx/pca95xx_registers.h"

#include <stdio.h>
#include <string.h>

/*! Array size, number of elements */
#define MAX_ARRAY(array)                (sizeof(array)/sizeof(array[0]))

/*! PCA95xx registers key:value map */
static const struct _reg {
    enum pca95xx_reg reg;
    const char* name;
}g_reg_table[] = {
    {PCA95XX_REG_INPUT,                 "input"},
    {PCA95XX_REG_OUTPUT,                "output"},
    {PCA95XX_REG_CONFIGURATION,         "configuration"},
    {PCA95XX_REG_POLARITY_INV,          "polarity_inv"},
    {PCA95XX_REG_MASK_INTERRUPT,        "mask_interrupt"}
};

enum pca95xx_reg pca95xx_get_register_by_name(const char* name) {
    if (NULL == name) {
        return PCA95XX_REG_UNSUPPORTED;
    }

    for (unsigned int i = 0; i < MAX_ARRAY(g_reg_table); ++i) {
        if (strcmp(g_reg_table[i].name, name) == 0) {
            return g_reg_table[i].reg;
        }
    }

    return PCA95XX_REG_UNSUPPORTED;
}

const char* pca95xx_get_register_name(enum pca95xx_reg reg) {
    for (unsigned int i = 0; i < MAX_ARRAY(g_reg_table); ++i) {
        if (g_reg_table[i].reg == reg) {
            return g_reg_table[i].name;
        }
    }

    return "unknown";
}
