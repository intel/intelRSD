/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file i2c_interface.hpp
 * @brief I2c interface
 * */

#pragma once

#include <string>
#include <memory>

/*! Agent namespace */
namespace agent {
/*! PNC namespace */
namespace pnc {
/*! I2c namespace */
namespace i2c {

/*! Access Interface to PNC's I2c bus */
class I2cAccessInterface {
public:
    /*! Default constructor */
    I2cAccessInterface() = default;

    /*! Copy constructor */
    I2cAccessInterface(const I2cAccessInterface&) = default;

    /*! Assignment operator */
    I2cAccessInterface& operator=(const I2cAccessInterface&) = default;

    /*!
     * @brief Method used to write on i2c.
     * @param port Port to write.
     * @param address Device address to write.
     * @param offset Device offset to write.
     * @param data Data to write.
     * @param size Size of the data to write.
     * @param is_16bit_offset If true 16-bit offset is used, otherwise (default) just 8-bits are taken
     */
    virtual void write(std::uint8_t port, std::uint16_t address, std::uint16_t offset, std::uint8_t* data,
        std::uint16_t size, bool is_16bit_offset = false) const = 0;

    /*!
     * @brief Method used to read from i2c.
     * @param port Port to read.
     * @param address Device address to read.
     * @param offset Device offset to read.
     * @param data Data to read.
     * @param size Size of the data to read.
     * @param is_16bit_offset If true 16-bit offset is used, otherwise (default) just 8-bits are taken
     */
    virtual void read(std::uint8_t port, std::uint16_t address, std::uint16_t offset, std::uint8_t* data,
        std::uint16_t size, bool is_16bit_offset = false) const = 0;

    virtual ~I2cAccessInterface();

};

using I2cAccessInterfacePtr = std::shared_ptr<I2cAccessInterface>;

}
}
}
