/*!
 * @brief System discoverer implementation.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License") override;
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file system_discoverer.cpp
 */

#include "agent-framework/discovery/discoverers/system_discoverer.hpp"
#include "agent-framework/discovery/builders/system_builder.hpp"



using namespace agent_framework::discovery;

namespace {
static constexpr const char GUID_PATH[] = "/sys/class/dmi/id/product_uuid";
}


agent_framework::model::System SystemDiscoverer::discover(const Uuid& parent_uuid,
                                                          const Uuid& chassis_uuid) {
    auto system = SystemBuilder::build_virtual(parent_uuid, chassis_uuid);

    try {
        auto guid_file = m_sysfs->get_file(GUID_PATH);
        SystemBuilder::update_with_guid(system, guid_file);
    }
    catch (const std::exception& exception) {
        log_error("system-discoverer", "Could not read system GUID from sysfs: " << exception.what());

    }

    return system;
}