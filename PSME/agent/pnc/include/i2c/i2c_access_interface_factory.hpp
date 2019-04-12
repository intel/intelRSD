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
 * @file i2c_interface_factory.hpp
 * @brief I2c interface factory
 * */

#pragma once

#include "i2c/i2c_access_interface.hpp"

/*! Agent namespace */
namespace agent {
/*! PNC namespace */
namespace pnc {
/*! GAS namespace */
namespace i2c {

/*! Creates I2cAccessInterface objects providing access to PNC's I2c bus */
class I2cAccessInterfaceFactory final {
public:
    /*! Default constructor */
    I2cAccessInterfaceFactory() {}

    /*! Default destructor */
    ~I2cAccessInterfaceFactory();

    /*!
     * @brief Gets I2c access interface
     * @return I2c Access Interface
     * */
    I2cAccessInterfacePtr get_interface();

    /*!
     * @brief Initializes gas interface of the factory
     * @param[in] path Path to the gas memory file
     * */
    void init_gas_interface(const std::string& path) {
        m_gas_path = path;
        m_gas_initialized = true;
    }

    /*!
     * @brief Initializes ipmi interface of the factory
      * @param[in] ip Ip address
      * @param[in] port port
      * @param[in] user UserId
      * @param[in] pass password
      * */
    void init_ipmi_interface(const std::string& ip, const std::uint32_t& port, const std::string& user,
            const std::string& pass) {
        m_ip = ip;
        m_port = port;
        m_user = user;
        m_pass = pass;
        m_ipmi_initialized = true;
    }

    /*!
     * @brief Switches factory to produce Gas based interfaces
     * */
    void switch_to_gas() {
        m_current_interface = I2cAccessInterfaceCode::Gas;
    }

    /*!
     * @brief Switches factory to produce Ipmi based interfaces
     * */
    void switch_to_ipmi() {
        m_current_interface = I2cAccessInterfaceCode::Ipmi;
    }

    /*!
     * @brief Returns reference to the default factory
     * @return I2cInterfaceFactory reference
     * */
    static I2cAccessInterfaceFactory& get_instance();

private:

    /*! I2c access interface enum codes */
    enum class I2cAccessInterfaceCode : std::uint8_t {
        None = 0,
        Gas = 1,
        Ipmi = 2
    };

    I2cAccessInterfaceCode m_current_interface{I2cAccessInterfaceCode::None};

    // GAS implementation variables
    std::string m_gas_path{};

    // IPMI implementation variables
    std::string m_ip{};
    std::uint32_t m_port{};
    std::string m_user{};
    std::string m_pass{};

    bool m_gas_initialized{false};
    bool m_ipmi_initialized{false};

};

}
}
}
