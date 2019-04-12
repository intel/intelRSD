/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 *
 * @file registration_response.hpp
 * @brief Registration response
 * */

#pragma once
#include <string>

namespace agent_framework {
namespace generic {

/*!
 * @brief Registration response containing AMC (Assets Management Core)
 * application's event listener ipv4 address and port number.
 */
class RegistrationResponse {
public:
    /*!
     * @brief Constructor
     * @param ip AMC application's event listener IP address.
     * @param port AMC application's event listener port.
     */
    RegistrationResponse(const std::string& ip, int port)
      : m_amc_event_listener_ip{ip}, m_amc_event_listener_port{port} {}

    /*!
     * @brief Gets AMC application's event listener IP address.
     * @return AMC application's event listener IP address.
     */
    std::string get_amc_event_listener_ip(void) const {
        return m_amc_event_listener_ip;
    }

    /*!
     * @brief Gets AMC application's event listener IP address.
     * @return AMC application's event listener IP address.
     */
    int get_amc_event_listener_port(void) const {
        return m_amc_event_listener_port;
    }

    /*!
     * @brief Gets AMC application's event listener url.
     * @return AMC application's event listener url.
     */
    std::string get_amc_event_listener_url() const {
        if (!m_amc_event_listener_ip.empty() && m_amc_event_listener_port > 0) {
            return "http://" + m_amc_event_listener_ip
                    + ":" + std::to_string(m_amc_event_listener_port);
        }
        return "";
    }

private:
    std::string m_amc_event_listener_ip{};
    int m_amc_event_listener_port{0};
};

}
}

