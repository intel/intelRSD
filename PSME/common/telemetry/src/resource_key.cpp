/*!
 * @brief Resource mapping implementation
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
 * @file resource_instance.cpp
 */

#include "telemetry/resource_key.hpp"

#include <unordered_map>

using telemetry::ResourceInstance;

namespace {

using ResourceMapping = std::unordered_map<std::string, ResourceInstance>;

ResourceMapping processor_mapping {
    /* GRANTLEY Socket identifiers */
    {"SOCKET 0", ResourceInstance{ResourceInstance::Component::Processor, 0}},
    {"SOCKET 1", ResourceInstance{ResourceInstance::Component::Processor, 1}},
    /* PURLEY Socket identifiers */
    {"CPU0", ResourceInstance{ResourceInstance::Component::Processor, 0}},
    {"CPU1", ResourceInstance{ResourceInstance::Component::Processor, 1}},
};

ResourceMapping memory_mapping {
    /* GRANTLEY Memory identifiers */
    {"A0", ResourceInstance{ResourceInstance::Component::Memory, 0}},
    {"A1", ResourceInstance{ResourceInstance::Component::Memory, 1}},
    {"A2", ResourceInstance{ResourceInstance::Component::Memory, 2}},
    {"A3", ResourceInstance{ResourceInstance::Component::Memory, 3}},
    {"A4", ResourceInstance{ResourceInstance::Component::Memory, 4}},
    {"A5", ResourceInstance{ResourceInstance::Component::Memory, 5}},
    {"A6", ResourceInstance{ResourceInstance::Component::Memory, 6}},
    {"A7", ResourceInstance{ResourceInstance::Component::Memory, 7}},
    {"B0", ResourceInstance{ResourceInstance::Component::Memory, 8}},
    {"B1", ResourceInstance{ResourceInstance::Component::Memory, 9}},
    {"B2", ResourceInstance{ResourceInstance::Component::Memory, 10}},
    {"B3", ResourceInstance{ResourceInstance::Component::Memory, 11}},
    {"B4", ResourceInstance{ResourceInstance::Component::Memory, 12}},
    {"B5", ResourceInstance{ResourceInstance::Component::Memory, 13}},
    {"B6", ResourceInstance{ResourceInstance::Component::Memory, 14}},
    {"B7", ResourceInstance{ResourceInstance::Component::Memory, 15}},
    /* PURLEY Memory identifiers */
    {"DIMM A0", ResourceInstance{ResourceInstance::Component::Memory, 0}},
    {"DIMM A1", ResourceInstance{ResourceInstance::Component::Memory, 1}},
    {"DIMM A2", ResourceInstance{ResourceInstance::Component::Memory, 2}},
    {"DIMM B0", ResourceInstance{ResourceInstance::Component::Memory, 3}},
    {"DIMM B1", ResourceInstance{ResourceInstance::Component::Memory, 4}},
    {"DIMM B2", ResourceInstance{ResourceInstance::Component::Memory, 5}},
    {"DIMM C0", ResourceInstance{ResourceInstance::Component::Memory, 6}},
    {"DIMM C1", ResourceInstance{ResourceInstance::Component::Memory, 7}},
    {"DIMM C2", ResourceInstance{ResourceInstance::Component::Memory, 8}},
    {"DIMM D0", ResourceInstance{ResourceInstance::Component::Memory, 9}},
    {"DIMM D1", ResourceInstance{ResourceInstance::Component::Memory, 10}},
    {"DIMM D2", ResourceInstance{ResourceInstance::Component::Memory, 11}},
    /* AMI BIOS (Purley refresh?) Memory identifiers*/
    {"CPU0_DIMM_A0", ResourceInstance{ResourceInstance::Component::Memory, 0}},
    {"CPU0_DIMM_A1", ResourceInstance{ResourceInstance::Component::Memory, 1}},
    {"CPU0_DIMM_A2", ResourceInstance{ResourceInstance::Component::Memory, 2}},
    {"CPU0_DIMM_B0", ResourceInstance{ResourceInstance::Component::Memory, 3}},
    {"CPU0_DIMM_B1", ResourceInstance{ResourceInstance::Component::Memory, 4}},
    {"CPU0_DIMM_B2", ResourceInstance{ResourceInstance::Component::Memory, 5}},
    {"CPU1_DIMM_C0", ResourceInstance{ResourceInstance::Component::Memory, 6}},
    {"CPU1_DIMM_C1", ResourceInstance{ResourceInstance::Component::Memory, 7}},
    {"CPU1_DIMM_C2", ResourceInstance{ResourceInstance::Component::Memory, 8}},
    {"CPU1_DIMM_D0", ResourceInstance{ResourceInstance::Component::Memory, 9}},
    {"CPU1_DIMM_D1", ResourceInstance{ResourceInstance::Component::Memory, 10}},
    {"CPU1_DIMM_D2", ResourceInstance{ResourceInstance::Component::Memory, 11}}
};

ResourceInstance get_resource_instance_by_key(const std::string& key, const ResourceMapping& mapping) {
    auto it = mapping.find(key);
    if (it != mapping.cend()) {
        return it->second;
    }
    return ResourceInstance{};
}

ResourceInstance get_resource_instance_by_key(const OptionalField<std::string>& key, const ResourceMapping& mapping) {
    return key.has_value() ? get_resource_instance_by_key(key.value(), mapping) : ResourceInstance{};
}

}


namespace telemetry {

std::ostream& operator<<(std::ostream& os, const ResourceInstance& resource_key) {
    os << "[" << resource_key.get_component().to_string();
    if (ResourceInstance::ANY_INSTANCE != resource_key.get_instance_number()) {
        os << ":" << resource_key.get_instance_number();
    }
    os << "]";
    return os;
}

template<>
ResourceInstance get_resource_key<agent_framework::model::Processor>(const agent_framework::model::Processor& processor) {
    return get_resource_instance_by_key(processor.get_socket(), processor_mapping);
}

template<>
ResourceInstance get_resource_key<agent_framework::model::Memory>(const agent_framework::model::Memory& memory) {
    return get_resource_instance_by_key(memory.get_device_locator(), memory_mapping);
}


}
