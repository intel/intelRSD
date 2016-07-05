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
 * @file pca95xx/pca95xx.h
 *
 * @brief PCA95XX GPIO expendar interface
 * */

#ifndef PCA95XX_H
#define PCA95XX_H

/*! PCA95XX GPIO expander model */
enum pca95xx_model {
    PCA95XX_UNKNOWN, /* Unknown model. Error? */
    PCA9505,
    PCA9506,
    PCA9555
};

/*! PCA95XX GPIO expander register */
enum pca95xx_reg {
    PCA95XX_REG_UNSUPPORTED, /* Unsupported register. Error? */
    PCA95XX_REG_INPUT,
    PCA95XX_REG_OUTPUT,
    PCA95XX_REG_CONFIGURATION,
    PCA95XX_REG_POLARITY_INV,
    PCA95XX_REG_MASK_INTERRUPT
};

/*! PCA95XX status code */
enum pca95xx_status {
    PCA95XX_SUCCESS                     =  0,
    PCA95XX_ERROR                       = -1,
    PCA95XX_ERROR_NULL                  = -2,
    PCA95XX_ERROR_I2C_OPEN              = -3,
    PCA95XX_ERROR_I2C_CLOSE             = -4,
    PCA95XX_ERROR_I2C_BUS_RANGE         = -5,
    PCA95XX_ERROR_I2C_ADDRESS           = -6,
    PCA95XX_ERROR_I2C_WRITE_READ        = -7,
    PCA95XX_ERROR_IO_BANK_RANGE         = -8,
    PCA95XX_ERROR_UNKNOWN_MODEL         = -9,
    PCA95XX_ERROR_UNKNOWN_REGISTER      = -11
};

/*! PCA95XX GPIO expander instance */
struct pca95xx;

/*! PCA95XX write handler */
struct pca95xx_write {
    int (*handler)(struct pca95xx* inst, enum pca95xx_reg reg,
            unsigned int bank, unsigned int value); /*!< Write handler */
};

/*! PCA95XX read handler */
struct pca95xx_read {
    int (*handler)(struct pca95xx* inst,
            enum pca95xx_reg reg, unsigned int bank); /*!< Read handler */
};

/*! PCA95XX IO operation handlers */
struct pca95xx_io {
    struct pca95xx_write write; /*!< IO write handler */
    struct pca95xx_read read;   /*!< IO read handler */
};

/*! PCA95XX device instance */
struct pca95xx_dev {
    const char* name;           /*!< Device name string */
    struct pca95xx_io io;       /*!< IO operation like write/read */
    enum pca95xx_model model;   /*!< PCA95xx model ID */
};

/*! PCA95XX I2C instance */
struct pca95xx_i2c {
    unsigned int bus_number;    /*!< I2C bus number */
    unsigned int slave_address; /*!< I2C slave address */
};

/*! PCA95XX GPIO expander instance */
struct pca95xx {
    struct pca95xx_dev dev; /*!< PCA95xx device implementation */
    struct pca95xx_i2c i2c; /*!< PCA95xx I2C implementation */
};

/*!
 * @brief Initialize PCA95xx structure
 *
 * Set proper IC behaviour based on model. It's required on start before
 * using PCA95xx API. It may be used for re-initizalization.
 *
 * @param[in,out]   inst    device instantion
 * @param[in]       model   IC model
 * @return          pca95xx_status or other negative error code (example from
 *                  third party library)
 * */
int pca95xx_init(struct pca95xx* inst, enum pca95xx_model model);

/*!
 * @brief Set IC bechaviour based on IC model.
 *
 * @param[in,out]   inst    device instantion
 * @param[in]       model   IC model ID
 * @return          pca95xx_status or other negative error code (example from
 *                  third party library)
 * */
int pca95xx_set_model(struct pca95xx* inst, enum pca95xx_model model);

/*!
 * @brief Get IC model ID based on IC model string.
 *
 * @param[in]       name    IC model string
 * @return          IC model ID
 * */
enum pca95xx_model pca95xx_get_model_by_name(const char* name);

/*!
 * @brief Select PCA95xx on I2C bus.
 *
 * The value is based on Ax pins not the real IC slave address.
 * For more information please see PCA95xx datasheet and electrical schematic
 * (or ask your hardware engineer)
 *
 * @param[in,out]   inst    PCA95xx device instantion
 * @param[in]       slave_address   I2C slave address
 * */
static inline void pca95xx_set_i2c_slave_address(struct pca95xx* inst,
        unsigned int slave_address) {
    inst->i2c.slave_address = slave_address;
}

/*!
 * @brief Choose I2C bus. 0 means first I2C peripheral.
 * Choose wisely.
 *
 * @param[in,out]   inst    PCA95xx device instantion
 * @param[in]       bus_number  I2C bus number. 0 means first I2C
 * */
static inline void pca95xx_set_i2c_bus_number(struct pca95xx* inst,
        unsigned int bus_number) {
    inst->i2c.bus_number = bus_number;
}

/*!
 * @brief Get PCA95xx model name
 *
 * @param[in,out]   inst    PCA95xx device instantion
 * @return          PCA95xx model name string
 * */
static inline const char* pca95xx_get_name(struct pca95xx* inst) {
    return inst->dev.name;
}

/*!
 * @brief Set IO bank output.
 *
 * @param[in,out]   inst    PCA95xx device instantion
 * @param[in]       bank    IO bank. 0 means first bank
 * @param[in]       value   Value that will be written to the IO bank
 * @return          pca95xx_status or other negative error code (example from
 *                  third party library)
 * */
static inline int pca95xx_set_output(struct pca95xx* inst,
        unsigned int bank, unsigned int value) {
    return inst->dev.io.write.handler(inst, PCA95XX_REG_OUTPUT, bank, value);
}

/*!
 * @brief Get IO bank input.
 *
 * @param[in,out]   inst    PCA95xx device instantion
 * @param[in]       bank    IO bank. 0 means first bank
 * @return          IO bank value between 0x00-0xFF otherwise error
 *                  (negative number)
 * */
static inline int pca95xx_get_input(struct pca95xx* inst,
        unsigned int bank) {
    return inst->dev.io.read.handler(inst, PCA95XX_REG_INPUT, bank);
}

/*!
 * @brief Set IO configuration.
 *
 * @param[in,out]   inst    PCA95xx device instantion
 * @param[in]       bank    IO bank. 0 means first bank
 * @param[in]       value   Value that will be written to the IO bank
 *                  0 - configure pin as input
 *                  1 - configure pin as output
 * @return          pca95xx_status or other negative error code (example from
 *                  third party library)
 * */
static inline int pca95xx_set_config(struct pca95xx* inst,
        unsigned int bank, unsigned int value) {
    return inst->dev.io.write.handler(inst, PCA95XX_REG_CONFIGURATION,
            bank, value);
}

/*!
 * @brief Get IO configuration.
 *
 * @param[in,out]   inst    PCA95xx device instantion
 * @param[in]       bank    IO bank. 0 means first bank
 * @return          Bank configuration otherwise error (negative number)
 *                  0 - pin configured as input
 *                  1 - pin configured as output
 * */
static inline int pca95xx_get_config(struct pca95xx* inst,
        unsigned int bank) {
    return inst->dev.io.read.handler(inst, PCA95XX_REG_CONFIGURATION, bank);
}

/*!
 * @brief Set IO polarity inversion.
 *
 * @param[in,out]   inst    PCA95xx device instantion
 * @param[in]       bank    IO bank. 0 means first bank
 * @param[in]       value   Value that will be written to the IO bank
 *                  0 - retain pin polarity (normal)
 *                  1 - invert pin polarity
 * @return          pca95xx_status or other negative error code (example from
 *                  third party library)
 * */
static inline int pca95xx_set_polarity_inv(struct pca95xx* inst,
        unsigned int bank, unsigned int value) {
    return inst->dev.io.write.handler(inst, PCA95XX_REG_POLARITY_INV,
            bank, value);
}

/*!
 * @brief Set IO polarity inversion.
 *
 * @param[in,out]   inst    PCA95xx device instantion
 * @param[in]       bank    IO bank. 0 means first bank
 * @return          Bank polarity settings otherwise error (negative number)
 *                  0 - pin polarity is retained (normal)
 *                  1 - pin polarity is inverted
 * */
static inline int pca95xx_get_polarity_inv(struct pca95xx* inst,
        unsigned int bank) {
    return inst->dev.io.read.handler(inst, PCA95XX_REG_POLARITY_INV, bank);
}

#endif /* PCA95XX_H */
