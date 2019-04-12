/*!
 * @brief Manager builder class interface.
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
 * @file manager_builder.hpp
 */

#pragma once



#include "agent-framework/module/model/manager.hpp"
#include "ipmi/command/generic/get_device_guid.hpp"
#include "ipmi/command/generic/get_device_id.hpp"



namespace agent {
namespace compute {
namespace discovery {

class ManagerBuilder {
public:
    /*!
     * @brief Build default manager object.
     * @return Default manager object.
     */
    static agent_framework::model::Manager build_default();


    /*!
     * @brief Update manager object with IPMI GetDeviceGuid command data.
     * @param manager Manager object to be filled with discovered data.
     * @param get_device_guid GetDeviceGuid IPMI command response object.
     */
    static void update_guid_data(agent_framework::model::Manager& manager,
                                 const ipmi::command::generic::response::GetDeviceGuid& get_device_guid);


    /*!
     * @brief Update manager object with IPMI GetDeviceId command data.
     * @param manager Manager object to be filled with discovered data.
     * @param get_device_id GetDeviceId IPMI command response object.
     */
    static void update_device_id_data(agent_framework::model::Manager& manager,
                                      const ipmi::command::generic::response::GetDeviceId& get_device_id);


    /*!
     * @brief Update manager with chassis UUID.
     * @param manager Manager object to be updated.
     * @param chassis_uuid Enclosing chassis UUID.
     */
    static void update_chassis(agent_framework::model::Manager& manager, const std::string& chassis_uuid);
};

}
}
}

