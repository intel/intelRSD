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

#include "pca9505.h"
#include "pca95xx/pca95xx_i2c.h"

#define MAX_ARRAY(array)                    (sizeof(array)/sizeof(array[0]))

/*! PCA9505 registers key:value map */
static const struct pca9505_reg {
    enum pca95xx_reg reg;
    unsigned int command;
}g_pca9505_reg_table[] = {
    {PCA95XX_REG_INPUT,             PCA9505_COMMAND_INPUT},
    {PCA95XX_REG_OUTPUT,            PCA9505_COMMAND_OUTPUT},
    {PCA95XX_REG_CONFIGURATION,     PCA9505_COMMAND_CONFIGURATION},
    {PCA95XX_REG_POLARITY_INV,      PCA9505_COMMAND_POLARITY_INV},
    {PCA95XX_REG_MASK_INTERRUPT,    PCA9505_COMMAND_MASK_INTERRUPT}
};

/*!
 * @brief Get PCA9505 command byte based on choosen register and
 * register bank
 *
 * @param[in]   reg     PCA9505 register
 * @param[in]   bank    Bank register. 0 means first bank etc.
 * @return      command byte when success otherwise error (negative number)
 * */
static int pca9505_get_command(enum pca95xx_reg reg,
        unsigned int bank) {

    for (unsigned int i=0; i<MAX_ARRAY(g_pca9505_reg_table); ++i) {
        if (g_pca9505_reg_table[i].reg == reg) {
            return (int)(g_pca9505_reg_table[i].command | bank);
        }
    }

    return PCA95XX_ERROR_UNKNOWN_REGISTER;
}

static inline int pca9505_check_address(unsigned int address) {
    return ((PCA9505_ADDRESS_MASK & address) == PCA9505_ADDRESS_BASIC) ?
        PCA95XX_SUCCESS : PCA95XX_ERROR;
}

int pca9505_write(struct pca95xx* inst, enum pca95xx_reg reg,
        unsigned int bank, unsigned int value) {

    if (pca9505_check_address(inst->i2c.slave_address) != PCA95XX_SUCCESS) {
        return PCA95XX_ERROR_I2C_ADDRESS;
    }
    if (!(bank < PCA9505_MAX_IO_BANKS)) {
        return PCA95XX_ERROR_IO_BANK_RANGE;
    }

    int command = pca9505_get_command(reg, bank);
    if (command < 0) {
        /* Error code */
        return command;
    }

    return pca95xx_i2c_write(inst->i2c.bus_number, inst->i2c.slave_address,
            (unsigned int)command, value);
}

int pca9505_read(struct pca95xx* inst, enum pca95xx_reg reg,
        unsigned int bank) {

    if (pca9505_check_address(inst->i2c.slave_address) != PCA95XX_SUCCESS) {
        return PCA95XX_ERROR_I2C_ADDRESS;
    }
    if (!(bank < PCA9505_MAX_IO_BANKS)) {
        return PCA95XX_ERROR_IO_BANK_RANGE;
    }

    int command = pca9505_get_command(reg, bank);
    if (command < 0) {
        /* Error code */
        return command;
    }

    return pca95xx_i2c_read(inst->i2c.bus_number, inst->i2c.slave_address,
            (unsigned int)command);
}
