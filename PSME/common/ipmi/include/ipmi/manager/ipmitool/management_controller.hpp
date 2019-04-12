/*!
 * @brief Implementation of ManagementController interface.
 *
 * Current implementation of communication over "lan" interface.
 * Class forwards "configuration" requests to internal connection data, each
 * change actually causes new connection.
 *
 * FIXME This class should be replaced with ipmi::ManagementController.
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
 * @file ipmitool/management_controller.hpp
 */

#pragma once



#include "ipmi/management_controller.hpp"



namespace ipmi {
namespace manager {
namespace ipmitool {

class ManagementController : public ipmi::ManagementController {

public:
    ManagementController(ipmi::ConnectionData::Ptr _connection_data) :
        ipmi::ManagementController(_connection_data) {}


    ManagementController(const std::string& ip, std::uint32_t port, const std::string& username,
                         const std::string& password) :
        ipmi::ManagementController(ip, port, username, password) {}


    ManagementController() = default;


    ManagementController(const ManagementController&) = default;


    ManagementController(ManagementController&&) = default;


    virtual ~ManagementController();
};

}
}
}
