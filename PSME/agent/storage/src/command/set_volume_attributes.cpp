/*!
 * @copyright
 * Copyright (c) 2017-2018 Intel Corporation
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
 * */

#include "command/set_volume_attributes.hpp"
#include "lvm/lvm_api.hpp"
#include "lvm/lvm_attribute.hpp"
#include "common/utils.hpp"

#include "agent-framework/module/requests/validation/storage.hpp"



using namespace agent::storage;
using namespace agent::storage::utils;
using namespace agent_framework;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::model::requests::validation;

namespace {

void process_bootable_tag(const Uuid& uuid, bool value) {
    auto volume = get_manager<Volume>().get_entry_reference(uuid);
    auto storage_pools = get_m2m_manager<StoragePool, Volume>().get_parents(volume->get_uuid());
    if (storage_pools.empty()) {
        THROW(agent_framework::exceptions::InvalidValue, "storage-agent", "Storage pool for volume not found!");
    }
    auto storage_pool = get_manager<StoragePool>().get_entry(storage_pools.front());

    lvm::LvmApi lvm_api{};
    auto vg_name = get_name_from_device_path(attribute::Identifier::get_system_path(storage_pool));
    auto lv_name = get_name_from_device_path(attribute::Identifier::get_system_path(volume.get_raw_ref()));

    if (value) {
        lvm_api.add_logical_volume_tag(vg_name, lv_name, lvm::attribute::LV_BOOTABLE_ATTR);
    }
    else {
        lvm_api.remove_logical_volume_tag(vg_name, lv_name, lvm::attribute::LV_BOOTABLE_ATTR);
    }

    volume->set_bootable(value);
    log_debug("storage-agent", "Set 'bootable' attribute to " << std::boolalpha << value << ".");
}

}


void agent::storage::process_volume_attributes(const Uuid& uuid, const attribute::Attributes& attributes,
                                               command::SetComponentAttributes::Response& response) {

    StorageValidator::validate_set_volume_attributes(attributes);
    if (attributes.empty()) {
        log_info("storage-agent", "Nothing has been changed (empty request).");
        return;
    }

    const auto attributes_names = attributes.get_names();
    for (const auto& attribute_name : attributes_names) {
        const auto& field_value = attributes.get_value(attribute_name);

        try {
            if (literals::Volume::BOOTABLE == attribute_name) {
                const auto& value = attributes.get_value(attribute_name);
                if (value.is_null()) {
                    THROW(exceptions::UnsupportedField, "storage-agent",
                          "Setting bootable flag to null is not supported.", attribute_name, field_value);
                }
                ::process_bootable_tag(uuid, value);
            }
            else {
                THROW(exceptions::UnsupportedField, "storage-agent",
                      "Setting attribute is not supported.", attribute_name, field_value);
            }
        }
        catch (const exceptions::GamiException& ex) {
            response.add_status({attribute_name, ex.get_error_code(), ex.get_message()});
        }
    }
}
