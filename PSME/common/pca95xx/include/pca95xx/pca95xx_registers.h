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
 * @file pca95xx/pca95xx_registers.h
 *
 * @brief PCA95XX registers interface
 * */

#ifndef PCA95XX_REGISTERS_H
#define PCA95XX_REGISTERS_H

#include "pca95xx/pca95xx.h"

/*!
 * @brief Get PCA95xx register ID by specify string register
 *
 * Mainly used for human interaction and debuging.
 *
 * @param[in]   name    Register string name
 * @return      Register ID
 * */
enum pca95xx_reg pca95xx_get_register_by_name(const char* name);

/*!
 * @brief Get PCA95xx register string by specify register ID
 *
 * Mainly used for human interaction and debuging.
 *
 * @param[in]   reg     Register ID
 * @return      Register string name
 * */
const char* pca95xx_get_register_name(enum pca95xx_reg reg);

#endif /* PCA95XX_REGISTERS_H */
