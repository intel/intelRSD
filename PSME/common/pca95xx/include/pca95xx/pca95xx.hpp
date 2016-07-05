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
 * @file pca95xx/pca95xx.hpp
 *
 * @brief PCA95XX C++ interaface
 * */

#ifndef PCA95XX_HPP
#define PCA95XX_HPP

#include <string>

/*! PCA95xx devices */
namespace pca95xx_cpp {

extern "C" {
#include "pca95xx/pca95xx.h"
}

/*!
 * @brief Class for handling PCA95xx GPIO expanders
 * */
class Pca95xx {
public:
    /*!
     * @brief Create PCA95xx device based on given model
     *
     * @param[in] model     PCA95xx device model
     * */
    Pca95xx(enum pca95xx_model model);

    /*!
     * @brief Set new device model
     *
     * @param[in] model     PCA95xxdevice model
     * */
    void set_model(enum pca95xx_model model);

    /*!
     * @brief Get device model ID by string
     *
     * @param[in] name      PCA95xx device model as string
     * @return              PCA95xx device model ID
     * */
    static enum pca95xx_model get_model(const char* name);

    /*!
     * @brief Get device model ID by string
     *
     * @param[in] name      PCA95xx device model as string
     * @return              PCA95xx device model ID
     * */
    static enum pca95xx_model get_model(const std::string& name);

    /*!
     * @brief Set I2C slave address
     *
     * @param[in] slave_address     I2C slave address
     * */
    void set_i2c_slave_address(unsigned int slave_address);

    /*!
     * @brief Set I2C bus number
     *
     * @param[in] bus_number        I2C bus number
     * */
    void set_i2c_bus_number(unsigned int bus_number);

    /*!
     * @brief Get device name
     *
     * @return Device name string
     * */
    std::string get_name();

    /*!
     * @brief Set GPIO output
     *
     * @param[in] bank      GPIO bank numered from 0 to ...
     * @param[in] value     GPIO bank value. Bit setting:
     *                      1 - IO pin is high
     *                      0 - IO pin is low
     * */
    void set_output(unsigned int bank, unsigned int value);

    /*!
     * @brief Get GPIO input
     *
     * @param[in] bank      GPIO bank numered from 0 to ...
     * @return              GPIO bank value. Bit setting:
     *                      1 - IO pin is high
     *                      0 - IO pin is low
     * */
    unsigned int get_input(unsigned int bank);

    /*!
     * @brief Set GPIO configuration
     *
     * @param[in] bank      GPIO bank numered from 0 to ...
     * @param[in] value     GPIO bank configuration. Bit setting:
     *                      0 - configure IO pin as input
     *                      1 - configure IO pin as output
     * */
    void set_config(unsigned int bank, unsigned int value);

    /*!
     * @brief Get GPIO configuration
     *
     * @param[in] bank      GPIO bank numered from 0 to ...
     * @return              GPIO bank configuration. Bit setting:
     *                      0 - IO pin configured as input
     *                      1 - IO pin configured as output
     * */
    unsigned int get_config(unsigned int bank);

    /*!
     * @brief Set GPIO bank polarity inverted options
     *
     * @param[in] bank      GPIO bank numered from 0 to ...
     * @param[in] value     GPIO bank polarity inverted options. Bit setting:
     *                      0 - retain pin polarity (normal)
     *                      1 - invert pin polarity
     * */
    void set_polarity_inv(unsigned int bank, unsigned int value);

    /*!
     * @brief Get GPIO bank polarity inverted options
     *
     * @param[in] bank      GPIO bank numered from 0 to ...
     * @return              GPIO bank polarity inverted options. Bit setting:
     *                      0 - retain pin polarity (normal)
     *                      1 - invert pin polarity
     * */
    unsigned int get_polarity_inv(unsigned int bank);
private:
    struct pca95xx m_pca95xx;
};

}

#endif /* PCA95XX_HPP */
