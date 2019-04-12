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
 * @file connection_data.hpp
 * @brief Connection data
 * */

#pragma once

#include <string>

namespace agent_framework {
namespace model {
namespace attribute {

/*! Connection data */
class ConnectionData {
public:

    explicit ConnectionData();

    ConnectionData(const std::uint32_t port,
                   const std::string& ip_address,
                   const std::string& username,
                   const std::string& password):
                   m_port{port},
                   m_ip_address{ip_address},
                   m_username{username},
                   m_password(password)
                   {}

    ~ConnectionData();

    /*! Enable copy */
    ConnectionData(const ConnectionData&) = default;
    ConnectionData& operator=(const ConnectionData&) = default;
    ConnectionData(ConnectionData&&) = default;
    ConnectionData& operator=(ConnectionData&&) = default;

    /*!
     * @brief Gets IP address of Module
     * @return IP address of module
     * */
    const std::string& get_ip_address() const {
        return m_ip_address;
    }

    /*!
     * @brief Gets user name
     * @return User name
     * */
    const std::string& get_username() const {
        return m_username;
    }

    /*!
     * @brief Gets password
     * @return Password
     * */
    const std::string& get_password() const {
        return m_password;
    }

    /*!
     * @brief Gets port
     * @return Port
     * */
    std::uint32_t get_port() const {
        return m_port;
    }

    /*!
     * @brief setter for ip_address attribute
     *
     * @param ip_address of type std::string
     */
    void set_ip_address(const std::string& ip_address) {
        m_ip_address = ip_address;
    }

    /*!
     * @brief setter for username attribute
     *
     * @param username of type std::string
     */
    void set_username(const std::string& username) {
        m_username = username;
    }

    /*!
     * @brief setter for password attribute
     *
     * @param password of type std::string
     */
    void set_password(const std::string& password) {
        m_password = password;
    }

    /*!
     * @brief setter for port attribute
     *
     * @param port of type std::uint32_t
     */
    void set_port(const std::uint32_t port) {
        m_port = port;
    }


private:
    std::uint32_t m_port{};
    std::string m_ip_address{};
    std::string m_username{};
    std::string m_password{};
};

}
}
}
