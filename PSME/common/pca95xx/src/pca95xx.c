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
 * @file pca95xx.c
 *
 * @brief PCA95XX GPIO expendar implementation
 * */

#include "pca95xx/pca95xx.h"
#include "pca95xx/pca95xx_i2c.h"
#include "pca95xx/pca95xx_common.h"

#include <string.h>
#include <stdio.h>

int pca95xx_init(struct pca95xx* inst, enum pca95xx_model model) {
    if (NULL == inst) {
        return PCA95XX_ERROR_NULL;
    }

    memset(inst, 0, sizeof(struct pca95xx));

    return pca95xx_set_model(inst, model);
}

int pca95xx_set_model(struct pca95xx* inst, enum pca95xx_model model) {
    int status = PCA95XX_ERROR_UNKNOWN_MODEL;
    const struct pca95xx_dev* const dev_table = pca95xx_dev_get_table();
    const unsigned int dev_table_size = pca95xx_dev_get_table_size();

    for (unsigned int i = 0; i < dev_table_size; ++i) {
        if (dev_table[i].model == model) {
            inst->dev = dev_table[i];
            status = PCA95XX_SUCCESS;
            break;
        }
    }

    if (status != PCA95XX_SUCCESS) {
        inst->dev.name = "unknown";
    }

    return status;
}

enum pca95xx_model pca95xx_get_model_by_name(const char* name) {
    if (NULL == name) {
        return PCA95XX_UNKNOWN;
    }

    const struct pca95xx_dev* const dev_table = pca95xx_dev_get_table();
    const unsigned int dev_table_size = pca95xx_dev_get_table_size();

    for (unsigned int i = 0; i < dev_table_size; ++i) {
        if (strcmp(dev_table[i].name, name) == 0) {
            return dev_table[i].model;
        }
    }

    return PCA95XX_UNKNOWN;
}
