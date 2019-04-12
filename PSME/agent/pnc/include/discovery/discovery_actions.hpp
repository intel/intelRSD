/*!
 * @brief Helper functions for adding, removing and updating resources in the model and logging those operations
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
 * @file discovery_actions.hpp
 */


#include "agent-framework/module/enum/pnc.hpp"
#include "agent-framework/module/managers/utils/manager_utils.hpp"



#pragma once

namespace agent {
namespace pnc {
namespace discovery {

/*!
 * @brief Helper function that removes the resource from the model and logs it
 * @tparam RESOURCE Type of resource to remove
 * @param uuid UUID of resource
 * @return Resource UUID
 */
template<typename RESOURCE>
Uuid log_and_remove(const std::string& uuid) {
    using RAW_TYPE = typename std::remove_reference<RESOURCE>::type;
    agent_framework::model::enums::Component component = RAW_TYPE::get_component();
    log_info("pnc-discovery", component.to_string() << " removed");
    log_debug("pnc-discovery", "Removed " << component.to_string() << " uuid: " << uuid);
    agent_framework::module::get_manager<RAW_TYPE>().remove_entry(uuid);
    return uuid;
}


/*!
 * @brief Helper function that adds the resource to the model and logs it
 * @tparam RESOURCE Type of resource to add
 * @param resource Resource to add
 * @return Resource UUID
 */
template<typename RESOURCE>
Uuid log_and_add(const RESOURCE& resource) {
    using RAW_TYPE = typename std::remove_reference<RESOURCE>::type;
    std::string uuid = resource.get_uuid();
    agent_framework::model::enums::Component component = RAW_TYPE::get_component();
    log_info("pnc-discovery", component.to_string() << " found");
    log_debug("pnc-discovery", component.to_string() << " uuid: " << uuid);
    agent_framework::module::get_manager<RAW_TYPE>().add_entry(resource);
    return uuid;
}


/*!
 * @brief Helper function that updates the resource in the model and logs it
 * @tparam RESOURCE Type of resource to update
 * @param resource Resource to uidate
 * @return Resource UUID
 */
template<typename RESOURCE>
Uuid log_and_update(const RESOURCE& resource) {
    using RAW_TYPE = typename std::remove_reference<RESOURCE>::type;
    std::string uuid = resource.get_uuid();
    agent_framework::model::enums::Component component = RAW_TYPE::get_component();
    log_debug("pnc-discovery", component.to_string() << " with uuid: " + uuid << " has been updated");
    agent_framework::module::get_manager<RAW_TYPE>().get_entry_reference(resource.get_uuid()).get_raw_ref() = resource;
    return uuid;
}

}
}
}