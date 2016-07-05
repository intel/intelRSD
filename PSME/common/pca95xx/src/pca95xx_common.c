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
 * @file pca95xx_common.c
 *
 * @brief PCA95XX common implementation
 * */

#include "pca95xx/pca95xx_common.h"

/* PCA95xx models */
#include "models/pca9505.h"
#include "models/pca9555.h"

/*!
 * @brief Common PCA95xx device table
 *
 * Please register all new models here and put IO functions in seperate source
 * files placed in models directory
 * */
static const struct pca95xx_dev g_pca95xx_dev_table[] = {
    {
        .name = "PCA9505",
        .model = PCA9505,
        .io = {.write = {pca9505_write}, .read = {pca9505_read}}
    },
    {
        .name = "PCA9506",
        .model = PCA9506,
        .io = {.write = {pca9505_write}, .read = {pca9505_read}}
    },
    {
        .name = "PCA9555",
        .model = PCA9555,
        .io = {.write = {pca9555_write}, .read = {pca9555_read}}
    }
};

const struct pca95xx_dev* pca95xx_dev_get_table(void) {
    return g_pca95xx_dev_table;
}

unsigned int pca95xx_dev_get_table_size(void) {
    return sizeof(g_pca95xx_dev_table)/sizeof(g_pca95xx_dev_table[0]);
}
