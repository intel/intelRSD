/*!
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
 *
 * @file /ipmi/management_controller.hpp
 *
 * @brief Interface for IPMI Management Controller. Implement this interface to
 * handle connections and sending/receiving commands.
 * */

#ifndef IPMI_MANAGEMENTCONTROLLER_HPP
#define IPMI_MANAGEMENTCONTROLLER_HPP

#include "request.hpp"
#include "response.hpp"

#include <memory>

namespace ipmi {

 /*!
 * @brief ManagementController interafce.
 * */
class ManagementController {
public:
    ManagementController();
    virtual ~ManagementController();

    /*!
     * Sends command to Management Controller.
     * @param request command to be send.
     * @param response buffer for output/response.
     */
    virtual void send(const Request & request, Response & response) = 0;

    /*!
     * Sets username to connect to management controller.
     * @param username string with username.
     */
    virtual void set_username(const std::string& username) {
        m_username = username;
    }

    /*!
     * Gets username used to connect to controller.
     * @return string with username.
     */
    virtual const std::string& get_username() const {
        return m_username;
    }

    /*!
     * Sets password to connect to management controller.
     * @param password string with password.
     */
    virtual void set_password(const std::string& password) {
        m_password = password;
    }

    /*!
     * Gets password used to connect to controller.
     * @return string with password.
     */
    virtual const std::string& get_password() const {
        return m_password;
    }

    /*!
     * Sets IP address to connect to management controller.
     * @param ip string with IP address.
     */
    virtual void set_ip(const std::string& ip) {
        m_ip = ip;
    }

    /*!
     * Gets IP address used to connect to management controller.
     * @return string with IP address.
     */
    virtual const std::string& get_ip() const {
        return m_ip;
    }

    /*!
     * Sets port number.
     * @param port UDP port number.
     */
    virtual void set_port(std::uint32_t port) {
        m_port = port;
    }

    /*!
     * Gets port number
     * @return UDP port number.
     */
    virtual std::uint32_t get_port() const {
        return m_port;
    }

private:
    std::string m_username{};
    std::string m_password{};
    std::string m_ip{};
    std::uint32_t m_port{};
};

}
#endif	/* MANAGEMENTCONTROLLER_HPP */

