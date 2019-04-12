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
 *
 *
 * @file responses/psme/attach.hpp
 * @brief Definition of PSME attach response
 * */

#pragma once

#include "json-wrapper/json-wrapper.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace responses {

/*! Attach response */
class Attach {
public:

    /*!
     * @brief Get IPv4 address of communication thread assigned to agent.
     * @return IPv4 address of communication thread assigned to agent.
     * */
    const std::string& get_ipv4_address() const {
        return m_ipv4_address;
    }

    /*!
     * @brief Set to response IPv4 address of communication thread assigned to agent.
     * @param ipv4_address New IPv4 address
     * */
    void set_ipv4_address(const std::string& ipv4_address) {
        m_ipv4_address = ipv4_address;
    }

    /*!
     * @brief Get port number of communication thread assigned to agent.
     * @return Port number of communication thread assigned to agent.
     * */
    int get_port() const {
        return m_port;
    }

    /*!
     * @brief Set to response TCP Port of communication
     * @param port New port value
     * */
    void set_port(const int port) {
        m_port = port;
    }

    /*!
     * @brief Get protocol version application is compatible with.
     * @return Protocol version application is compatible with.
     * */
    const std::string& get_version() const {
        return m_version;
    }

    /*!
     * @brief Set to response version of Application
     * @param version New version value
     * */
    void set_version(const std::string& version) {
        m_version = version;
    }

    /*!
     * @brief Transform response to Json
     * @return created Json value
     */
    json::Json to_json() const;

    /*!
     * @brief create Attach from Json
     * @param[in] json the input argument
     * @return new Attach
     */
    static Attach from_json(const json::Json& json);

private:
    std::string m_ipv4_address{};
    std::string m_version{};
    int m_port{};

};

}
}
}
