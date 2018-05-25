/*!
 * @header{License}
 * @copyright Copyright (c) 2017-2018 Intel Corporation.
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
 * @header{Filesystem}
 * @file base_fabric_interface.hpp
 */

#pragma once

#include "fabric_properties.hpp"

#include <string>
#include <vector>



namespace nvmfpp {

class BaseFabricInterface {
public:
    virtual ~BaseFabricInterface();

    /*!
     * @brief Connects to nvme drive.
     * @param fabric_properties Required and optional parameters to setup connection.
     */
    virtual void connect(const nvmfpp::FabricProperties& fabric_properties) = 0;

    /*!
     * @brief Disconnects only one nvme drive by name.
     * @param fabric_properties Required parameter to break the connection.
     */
    virtual void disconnect_by_drive(const nvmfpp::FabricProperties& fabric_properties) = 0;

    /*!
     * @brief Disconnects nvme drive by subsystem nqn. Could disconnect more then one drive.
     * @param fabric_properties Required parameter to break the connection.
     * @param drives Required list of all nvme drives name on host.
     */
    virtual void disconnect_by_nqn(const nvmfpp::FabricProperties& fabric_properties, const std::vector<std::string>& drives) = 0;

};

}
