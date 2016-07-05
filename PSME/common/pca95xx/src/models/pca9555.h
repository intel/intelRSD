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
 * */

#ifndef PCA9555_H
#define PCA9555_H

#include "pca95xx/pca95xx.h"

/* PCA9555 */
#define PCA9555_MAX_IO_BANKS                2
#define PCA9555_ADDRESS_BASIC               0x20
#define PCA9555_ADDRESS_MASK                0xF8
#define PCA9555_COMMAND_INPUT               0x00
#define PCA9555_COMMAND_OUTPUT              0x02
#define PCA9555_COMMAND_POLARITY_INV        0x04
#define PCA9555_COMMAND_CONFIGURATION       0x06

extern int pca9555_write(struct pca95xx* inst, enum pca95xx_reg reg,
        unsigned int bank, unsigned int value);
extern int pca9555_read(struct pca95xx* inst, enum pca95xx_reg reg,
        unsigned int bank);

#endif /* PCA9555_H */
