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
 * @file ipmitool/lan_connection_data.cpp
 */

#include "ipmi/manager/ipmitool/lan_connection_data.hpp"

using namespace ipmi::manager::ipmitool;

LanConnectionData::LanConnectionData() : ipmi::ConnectionData(LanConnectionData::INTF_TYPE) { }

LanConnectionData::LanConnectionData(const std::string& ip, std::uint32_t port,
                                     const std::string& username, const std::string& password):
    ipmi::ConnectionData(LanConnectionData::INTF_TYPE),
    m_ip(ip), m_port(port), m_username(username), m_password(password) { }

LanConnectionData::~LanConnectionData() { }


std::string LanConnectionData::get_info() const {
    return m_ip + ":" + std::to_string(m_port);
}
