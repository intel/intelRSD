/*!
 * @brief A scope locked style ipmi controller implementation.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file locked_management_controller.cpp
 */

#include "ipmi/manager/ipmitool/locked_management_controller.hpp"

namespace ipmi {
namespace manager {
namespace ipmitool {

LockedManagementController::LockedManagementController(ipmi::ConnectionData::Ptr _connection_data) :
    ManagementController(_connection_data) {
    // RAII style locked IPMI interface
    lock();
}

LockedManagementController::LockedManagementController(const std::string& ip, std::uint32_t port,
    const std::string& username, const std::string& password) :
    ManagementController(ip, port, username, password) {
    // RAII style locked IPMI interface
    lock();
}

LockedManagementController::LockedManagementController(const ManagementController& mc) :  ManagementController(mc) {
    // RAII style locked IPMI interface
    lock();
}

LockedManagementController::LockedManagementController() : ManagementController() {
    // RAII style locked IPMI interface
    lock();
}

LockedManagementController::~LockedManagementController() {
    try {
        unlock();
    } catch (...) {
        // eat the exception, no cleanup possible at this point
    }
}

void LockedManagementController::send(const Request& request, Response& response) {
    send_unlocked(request, response);
}

void LockedManagementController::send(const Request& request, const BridgeInfo& via, Response& response) {
    send_unlocked(request, via, response);
}



} // ipmitool
} // manager
} // ipmi
