/*!
 * @brief I2C message interface.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file i2c_message.hpp
 */

#pragma once



#include <cstdint>

/*! Agent namspace */
namespace agent {
/*! Compute namspace */
namespace compute {
/*! IPMB namspace */
namespace ipmb {

using uint8_t = std::uint8_t;
using uint32_t = std::uint32_t;

/*! I2C message */
class I2cMessage {
private:
    uint8_t m_slave_address{};
    uint32_t m_length{};
    uint8_t m_data{};

public:
    /*! Constructor /
     *  @param slave_address Slave address
     *  @param length Length
     *  @param data Data
     * */
    I2cMessage(uint8_t slave_address, uint32_t length, uint8_t data) :
        m_slave_address(slave_address), m_length(length), m_data(data) {
    }


    /*! Gets slave address
     *  @return Slave address. */
    uint8_t get_slave_address() const {
        return m_slave_address;
    }


    /*! Sets slave address
     *  @param slave_address Slave address
     * */
    void set_slave_address(const uint8_t slave_address) {
        m_slave_address = slave_address;
    }


    /*! Gets data
     *  @return data */
    uint8_t get_data() const {
        return m_data;
    }


    /*! Sets data
     *  @param data Data */
    void set_data(const uint8_t data) {
        m_data = data;
    }


    /*! Gets length
     *  @return Length */
    uint32_t get_length() const {
        return m_length;
    }


    /*! Sets length
     *  @param length Length */
    void set_length(const uint32_t length) {
        m_length = length;
    }
};

/*! IPMI Message shared pointer */
using I2cMessageSharedPtr = std::shared_ptr<I2cMessage>;

}
}
}

