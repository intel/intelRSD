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
 * @file pca95xx/pca95xx_common.h
 *
 * @brief PCA95XX common interface
 * */

#ifndef PCA95XX_COMMON_H
#define PCA95XX_COMMON_H

#include "pca95xx/pca95xx.h"

/*!
 * @brief Get PCA95xx device table
 *
 * @return PCA95xx device table
 * */
const struct pca95xx_dev* pca95xx_dev_get_table(void);

/*!
 * @brief Get PCA95xx device table size
 *
 * @return PCA95xx device table size
 * */
unsigned int pca95xx_dev_get_table_size(void);

#endif /* PCA95XX_COMMON_H */
