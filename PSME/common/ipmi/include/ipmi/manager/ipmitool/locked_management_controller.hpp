/*!
 * @brief A scope locked style ipmi controller.
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
 * @file locked_management_controller.hpp
 */

#pragma once

#include "ipmi/manager/ipmitool/management_controller.hpp"


namespace ipmi {
namespace manager {
namespace ipmitool {

/*!
 * This is a scope locked implementation of the ManagementController.
 */
class LockedManagementController final : public ManagementController {
public:

    /*!
     * @brief Constructor
     *
     * @param _connection_data The ipmi connection data.
     */
    LockedManagementController(ipmi::ConnectionData::Ptr _connection_data);

    /*!
     * @brief Constructor
     *
     * @param ip The ip address.
     * @param port The port number.
     * @param username The user name.
     * @param password The password.
     */
    LockedManagementController(const std::string& ip, std::uint32_t port, const std::string& username,
                         const std::string& password);

    /*!
     * @brief Constructor
     *
     * Explicit converting constructor from a ManagementController instance. The copied from ManagementController
     * should not be used within the scope of the LockedManagementController.
     *
     * @param mc The ManagementController instance.
     */
    explicit LockedManagementController(const ManagementController& mc);

    /*!
     * @brief Default constructor.
     */
    LockedManagementController();

    /*!
     * @brief Deleted copy constructor.
     */
    LockedManagementController(const LockedManagementController&) = delete;

    /*!
     * @brief Deleted move constructor.
     */
    LockedManagementController(LockedManagementController&&) = delete;

    /*!
     * @brief Deleted copy assignment operator.
     */
    LockedManagementController& operator=(const LockedManagementController&) = delete;

    /*!
     * @brief Deleted move assignment operator.
     */
    LockedManagementController& operator=(LockedManagementController&&) = delete;

    /*!
     * @brief Send message using bridge info.
     *
     * Default implementation with bridging.
     *
     * @param request message to be sent
     * @param via information about bridges
     * @param response received response
     */
    void send(const Request& request, const BridgeInfo& via, Response& response) override;

    /*!
     * @brief Send a message.
     *
     * Default implementation without bridging.
     *
     * @param request message to be sent.
     * @param response message with output/result.
     */
    void send(const Request& request, Response& response) override;

    /*!
     * @brief Destructor.
     */
    ~LockedManagementController();
};

} // ipmitool
} // manager
} // ipmi
