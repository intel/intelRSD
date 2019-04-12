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
 * @file ipmi_i2c_interface.hpp
 * @brief IPMI based I2c interface
 * */

#pragma once

#include "i2c/i2c_access_interface.hpp"

#include "ipmi/manager/ipmitool/management_controller.hpp"

/*! Agent namespace */
namespace agent {
/*! PNC namespace */
namespace pnc {
/*! I2c namespace */
namespace i2c {

/*! IMPI based Access Interface to PNC's I2c bus */
class IpmiI2cAccessInterface final : public I2cAccessInterface {
public:

    /*!
     * @brief Default constructor
     * @param[in] ip Ip address
     * @param[in] port port
     * @param[in] user UserId
     * @param[in] pass password
     * */
    IpmiI2cAccessInterface(const std::string& ip, const std::uint32_t& port, const std::string& user,
        const std::string& pass);

    /*! Copy constructor */
    IpmiI2cAccessInterface(const IpmiI2cAccessInterface&) = default;

    /*! Assignment operator */
    IpmiI2cAccessInterface& operator=(const IpmiI2cAccessInterface&) = default;

    /*! Destructor  */
    virtual ~IpmiI2cAccessInterface();

    /*!
     * @brief Method used to write on i2c.
     * @param port Port to write.
     * @param address Device address to write (limited to 8 bits).
     * @param offset Device offset to write (not implemented for IPMI interface).
     * @param data Data to write.
     * @param size Size of the data to write (limited to 8 bits).
     * @param is_16bit_offset If true 16-bit offset is used, otherwise (default) just 8-bits are taken
     */
    virtual void write(std::uint8_t port, std::uint16_t address, std::uint16_t offset, std::uint8_t* data,
        std::uint16_t size, bool is_16bit_offset = false) const override;

    /*!
     * @brief Method used to read from i2c.
     * @param port Port to read.
     * @param address Device address to read (limited to 8 bits).
     * @param offset Device offset to read.
     * @param data Data to read.
     * @param size Size of the data to read.
     * @param is_16bit_offset If true 16-bit offset is used, otherwise (default) just 8-bits are taken
     */
    virtual void read(std::uint8_t port, std::uint16_t address, std::uint16_t offset, std::uint8_t* data,
        std::uint16_t size, bool is_16bit_offset = false) const override;

private:
    std::shared_ptr<ipmi::ManagementController> m_mc{};
};

}
}
}
