/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 */

#pragma once



#include "json-wrapper/json-wrapper.hpp"
#include <string>



namespace agent_framework {
namespace generic {

class RegistrationData {
public:

    /*!
     * @brief Constructor.
     * @param config Configuration JSON object.
     */
    RegistrationData(const json::Json& config);


    /*! @brief Destructor. */
    virtual ~RegistrationData();


    /*!
     * @brief Gets IP address of the AMC application.
     * @return IPv4 address.
     */
    const std::string& get_ipv4_address() const {
        return m_ipv4_address;
    }


    /*!
     * @brief Gets port of the AMC application
     * @return AMC application port
     */
    std::uint16_t get_port() const {
        return m_port;
    }


    /*!
     * @brief Gets registration interval in seconds
     * @return Registration interval in seconds
     */
    std::uint32_t get_interval() const {
        return m_interval;
    }


private:
    std::string m_ipv4_address{};
    std::uint16_t m_port{};
    std::uint32_t m_interval{};
};

}
}
