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
 * @file include/ipmi/ipmitool/management_controller.hpp
 *
 * @brief Implementation of IPMI interface using ipmitool sources. Sends
 * synchronus messages to MC.
 * */

#ifndef AGENT_IPMI_IPMITOOL_MANAGEMENT_CONTROLLER_HPP
#define AGENT_IPMI_IPMITOOL_MANAGEMENT_CONTROLLER_HPP

#include "ipmi/management_controller.hpp"
extern "C" {
    struct ipmi_intf;
}

namespace ipmi {
namespace manager {
namespace ipmitool {

/*!
 * @brief Implementation of ManagementController interface basing on ipmitool
 * application source code solutions.
 */
class ManagementController: public ipmi::ManagementController {
public:

    /*!
     * @brief Default constructor.
     */
    ManagementController() = default;

    /*!
     * @brief Removed copy constructor.
     */
    ManagementController(const ManagementController&) = delete;

    /*!
     * @brief Default destructor.
     */
    virtual ~ManagementController() = default;

    virtual void send(const Request& request, Response& response);

private:
    // Currently sending each IPMI command may take up to 8 seconds (8 tries, 1 second timeout each)
    // This should be adjusted to comply with higher layers timeouts.
    // By default it was set to 4 tries, 15 seconds each - however in case of problems it would take
    // 60s to finish, which is way too long.
    std::uint32_t m_timeout_seconds{1};
    int m_retry_no{8};
    void init_interface(ipmi_intf* interface);
};

}
}
}
#endif	/* AGENT_IPMI_OPENIPMI_MANAGEMENT_CONTROLLER_HPP */
