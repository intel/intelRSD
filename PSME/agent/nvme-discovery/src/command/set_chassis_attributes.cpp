/*!
 * @copyright Copyright (c) 2017-2019 Intel Corporation
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
 * @file set_chassis_attributes.cpp
 */

#include "command/set_chassis_attributes.hpp"
#include "agent-framework/module/requests/validation/common.hpp"
#include "tools/databases.hpp"

#include <mutex>

using namespace agent::nvme;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework::exceptions;

namespace {

void set_asset_tag(const Uuid& uuid, const json::Json& value) {
    static std::mutex local_mutex{};
    std::lock_guard<std::mutex> lock(local_mutex);

    log_debug("nvme-discovery-agent", "Setting assetTag");
    tools::ChassisDatabase db{uuid};
    db.put(tools::NvmeDatabase::CHASSIS_ASSET_TAG, value);
    {
        auto chassis_ref = get_manager<Chassis>().get_entry_reference(uuid);
        chassis_ref->set_asset_tag(value);
    }
    log_debug("nvme-discovery-agent", "assetTag has been set to: " << value.dump());
}

}

void agent::nvme::command::set_chassis_attributes(agent::nvme::SetComponentAttributes::ContextPtr /*context*/,
        const Uuid& uuid, const agent_framework::model::attribute::Attributes& attributes,
        agent::nvme::SetComponentAttributes::Response& /*response*/) {

    log_debug("nvme-discovery-agent", "Executing setChassisAttributes command");
    log_debug("nvme-discovery-agent", "Processing Chassis [UUID = " << uuid << "]");
    requests::validation::CommonValidator::validate_set_chassis_attributes(attributes);

    for (const auto& attribute_name : attributes.get_names()) {
        const auto& value = attributes.get_value(attribute_name);

        log_debug("nvme-discovery-agent", "Attribute name: " << attribute_name);
        log_debug("nvme-discovery-agent", "Attribute value: " << value.dump());

        if (literals::Chassis::ASSET_TAG == attribute_name) {
            try {
                set_asset_tag(uuid, value);
            }
            catch (const std::exception& e) {
                THROW(NvmeError, "nvme-discovery-agent", std::string{"Error while setting chassis attributes: "} + e.what());
            }
        }
        else {
            // GeoTag, Reset, LocationId, Oem, all these fields are not supported
            THROW(UnsupportedField, "nvme-discovery-agent", "Setting attribute is not supported.", attribute_name, value);
        }
    }
}
