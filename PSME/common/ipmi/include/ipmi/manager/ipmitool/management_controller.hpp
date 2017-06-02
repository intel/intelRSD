/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * @brief Implementation of IPMI interface using ipmitool sources.
 * Sends synchronous messages to ManagementController.
 * */

#pragma once

#include "ipmi/management_controller.hpp"

namespace ipmi {
namespace manager {
namespace ipmitool {

/*!
 * @brief Implementation of ManagementController interface.
 * Based on ipmitool application source code solutions.
 */
class ManagementController: public ipmi::ManagementController {
public:

    /*! @brief Default constructor. */
    ManagementController() : ipmi::ManagementController() { }

    ManagementController(const ManagementController&) = default;
    ManagementController& operator=(const ManagementController&) = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~ManagementController() = default;

    void send(const Request& request, Response& response) override;

    /*! Sets ipmi interface type
     * @param[in] ipmi_interface_type Type of ipmi interface
     */
    void set_ipmi_interface_type(const std::string& ipmi_interface_type) {
        m_ipmi_interface_type = ipmi_interface_type;
    }

    /*! Gets ipmi interface type
     * @return Type of ipmi interface
     */
    const std::string& get_ipmi_interface_type() const {
        return m_ipmi_interface_type;
    }

private:
    std::string m_ipmi_interface_type{"lan"};
};

/*! Represents IPMI interface to Management Controller */
class IpmiInterface {
public:
    using UPtr = std::unique_ptr<IpmiInterface>;

    /*! Destructor */
    virtual ~IpmiInterface();

    /*! Opens connection to MC */
    virtual void open() = 0;

    /*! Closes connection to MC */
    virtual void close() = 0;

    /*! Sends IPMI request to MC
     * @param[in] request IPMI request
     * @param[out] response IPMI response
     */
    virtual void send(const Request& request, Response& response) = 0;

    /*! Sends message to keep IPMI session alive. */
    virtual int send_keep_alive() = 0;
};

/*!
 * IpmiInterface factory function.
 * @return IpmiInterface instance.
 */
IpmiInterface::UPtr create_ipmi_interface(const ManagementController& mc);

}
}
}
