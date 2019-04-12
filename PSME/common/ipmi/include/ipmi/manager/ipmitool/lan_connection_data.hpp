/*!
 * @brief "Lan" ManagementController configuration data.
 *
 * No default values are available, everything must be passed via ManagementController.
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
 * @file ipmitool/lan_connection_data.hpp
 */

#pragma once

#include <ipmi/connection_data.hpp>

#include <chrono>

namespace ipmi {
namespace manager {
namespace ipmitool {

/*!
 * @brief ManagementController interface.
 *
 * No default values are available, everything must be passed via ManagementController.
 */
class LanConnectionData : public ipmi::ConnectionData {
public:
    constexpr static const char* INTF_TYPE = "lan";

    LanConnectionData(const std::string& ip, std::uint32_t port, const std::string& username,
                      const std::string& password);

    LanConnectionData();
    LanConnectionData(const LanConnectionData&) = default;
    LanConnectionData(LanConnectionData&&) = default;
    virtual ~LanConnectionData();

    /*!
     * @brief Set username to connect to management controller.
     * @param username string with username.
     */
    void set_username(const std::string& username) {
        m_username = username;
    }


    /*!
     * @brief Get username used to connect to controller.
     * @return string with username.
     */
    const std::string& get_username() const {
        return m_username;
    }


    /*!
     * @brief Set password to connect to management controller.
     * @param password string with password.
     */
    void set_password(const std::string& password) {
        m_password = password;
    }


    /*!
     * @brief Get password used to connect to controller.
     * @return string with password.
     */
    const std::string& get_password() const {
        return m_password;
    }


    /*!
     * @brief Set IP address to connect to management controller.
     * @param ip string with IP address.
     */
    void set_ip(const std::string& ip) {
        m_ip = ip;
    }


    /*!
     * @brief Get IP address used to connect to management controller.
     * @return string with IP address.
     */
    const std::string& get_ip() const {
        return m_ip;
    }


    /*!
     * @brief Set port number.
     * @param port UDP port number.
     */
    void set_port(std::uint32_t port) {
        m_port = port;
    }


    /*!
     * @brief Get port number
     * @return UDP port number.
     */
    std::uint32_t get_port() const {
        return m_port;
    }

    /*!
     * @brief Set session timeout
     *
     * Setting timeout to 0 causes connection is not reused, this is each
     * message is sent in separate connection.
     *
     * @param timeout session timeout
     */
    void set_session_timeout(std::chrono::seconds timeout) {
        m_session_timeout = timeout;
    }


    /*!
     * @brief Get sessssion timeout
     * @return session timeout
     */
    std::chrono::seconds get_session_timeout() const {
        return m_session_timeout;
    }

    std::string get_info() const override;

private:
    std::string m_ip{};
    std::uint32_t m_port{623};
    std::string m_username{};
    std::string m_password{};
    std::chrono::seconds m_session_timeout{30};
};

}
}
}
