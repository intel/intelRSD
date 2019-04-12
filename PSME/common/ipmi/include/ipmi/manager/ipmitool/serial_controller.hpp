/*!
 * @brief Implementation of IpmiController interface for serial-oem.
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
 * @file ipmitool/serial_controller.hpp
 */

#pragma once

#include "ipmi/ipmi_controller.hpp"
#include "ipmi/manager/ipmitool/serial_connection_data.hpp"

namespace ipmi {
namespace manager {
namespace ipmitool {

class SerialController: public ipmi::IpmiController {

public:
    /*!
     * @brief Serial controller doesn't follow managementController pattern..
     *
     * There's no set-property() methods
     */
    SerialController() = delete;

    /*!
     * Destructor
     */
    virtual ~SerialController();

    /*!
     * @brief Default constructor
     * @param device path to the device to be used
     */
    SerialController(const std::string& device) :
        ipmi::IpmiController(std::make_shared<SerialConnectionData>(device)) { }

    /*!
     * @brief "Compability" constructor to build serial
     * @param _connection_data configuration data to be used
     */
    SerialController(ConnectionData::Ptr _connection_data):
        ipmi::IpmiController(_connection_data) {

        if (data()->get_interface_type() != SerialConnectionData::INTF_TYPE) {
            throw std::runtime_error("Cannot create serial controller on " + data()->get_interface_type() + " config");
        }
    }

};

}
}
}
