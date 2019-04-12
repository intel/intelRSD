/*!
 * @brief Interface class to give access to "lan" management controller.
 *
 * It uses Lan connection data (ip:port, user/pass). Interface is exposed.
 * Actually it isn't interface class (as expected). It has implementation
 * necessary for all derived classes (mocks), just backward compability.
 *
 * TODO move to ipmi::manager::ipmitool, use IpmiController as interface instead.
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
 * @file ipmi/management_controller.hpp
 */

#pragma once



#include "ipmi/manager/ipmitool/lan_connection_data.hpp"
#include "ipmi/ipmi_controller.hpp"



namespace ipmi {

class ManagementController : public ipmi::IpmiController {
public:
    ManagementController() :
        ipmi::IpmiController(std::make_shared<ipmi::manager::ipmitool::LanConnectionData>()) {}


    ManagementController(const std::string& ip, std::uint32_t port, const std::string& username,
                         const std::string& password) :
        ipmi::IpmiController(std::make_shared<ipmi::manager::ipmitool::LanConnectionData>()) {

        data_to_modify().set_ip(ip);
        data_to_modify().set_port(port);
        data_to_modify().set_username(username);
        data_to_modify().set_password(password);
    }


    ManagementController(ConnectionData::Ptr _connection_data) :
        ipmi::IpmiController(_connection_data) {}


    ManagementController(const ManagementController&) = default;


    ManagementController(ManagementController&&) = default;


    virtual ~ManagementController();


    /*!
     * Sets username to connect to management controller.
     * @param username string with username.
     */
    void set_username(const std::string& username) {
        data_to_modify().set_username(username);
    }


    /*!
     * Gets username used to connect to controller.
     * @return string with username.
     */
    const std::string& get_username() const {
        return data().get_username();
    }


    /*!
     * Sets password to connect to management controller.
     * @param password string with password.
     */
    void set_password(const std::string& password) {
        data_to_modify().set_password(password);
    }


    /*!
     * Gets password used to connect to controller.
     * @return string with password.
     */
    const std::string& get_password() const {
        return data().get_password();
    }


    /*!
     * Sets IP address to connect to management controller.
     * @param ip string with IP address.
     */
    void set_ip(const std::string& ip) {
        data_to_modify().set_ip(ip);
    }


    /*!
     * Gets IP address used to connect to management controller.
     * @return string with IP address.
     */
    const std::string& get_ip() const {
        return data().get_ip();
    }


    /*!
     * Sets port number.
     * @param port UDP port number.
     */
    void set_port(std::uint32_t port) {
        data_to_modify().set_port(port);
    }


    /*!
     * Gets port number
     * @return UDP port number.
     */
    std::uint32_t get_port() const {
        return data().get_port();
    }


private:
    const ipmi::manager::ipmitool::LanConnectionData& data() const {
        return dynamic_cast<const ipmi::manager::ipmitool::LanConnectionData&>(*ipmi::IpmiController::data());
    }


    ipmi::manager::ipmitool::LanConnectionData& data_to_modify() {
        return dynamic_cast<ipmi::manager::ipmitool::LanConnectionData&>(*ipmi::IpmiController::data_to_modify());
    }
};

}
