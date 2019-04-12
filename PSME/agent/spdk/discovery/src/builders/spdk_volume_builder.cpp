/*!
 * @brief SPDK Volume builder class implementation.
 *
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation.
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
 * @file spdk_volume_builder.cpp
 */

#include "agent-framework/discovery/builders/identifier_builder.hpp"
#include "discovery/builders/spdk_volume_builder.hpp"
#include "utils/string_utils.hpp"
#include "spdk/model/drivers/lvol_driver.hpp"



using namespace agent_framework;
using namespace agent_framework::discovery;
using namespace agent::spdk::discovery;

namespace {

static constexpr const char VOLUME_DESCRIPTION[] = "SPDK Logical Volume Block Device (LVOL BDEV)";

}

void SpdkVolumeBuilder::update_with_spdk(model::Volume& volume, const ::spdk::model::Bdev& lvol_bdev) {
    volume.set_parent_type(model::enums::Component::StorageService);
    volume.set_description(::VOLUME_DESCRIPTION);
    volume.set_block_size_bytes(lvol_bdev.get_block_size());

    auto lvol_driver = lvol_bdev.get_driver_specific<::spdk::model::drivers::LvolDriver>();
    model::attribute::Capacity capacity{};
    capacity.set_allocated_bytes(lvol_bdev.get_num_blocks() * lvol_bdev.get_block_size());
    if(lvol_driver) {
        capacity.set_thin_provisioned(lvol_driver->is_thin_provision());
    }
    volume.set_capacity(capacity);

    // Both get_uuid() and get_name() returns the same volume's UUID
    volume.set_name(lvol_bdev.get_name());

    // Clear current settings
    volume.set_access_capabilities(model::attribute::Array<model::enums::AccessCapability>{});

    if (lvol_bdev.get_supported_io_types().is_read()) {
        volume.add_access_capability(model::enums::AccessCapability::Read);
    }
    if (lvol_bdev.get_supported_io_types().is_write()) {
        volume.add_access_capability(model::enums::AccessCapability::Write);
    }

    model::attribute::Status status{};
    status.set_health(model::enums::Health::OK);
    status.set_state(model::enums::State::Enabled);
    volume.set_status(status);

    if (!lvol_bdev.get_aliases().empty()) {
        // Alias is in format: "LvolStoreName/LvolBdevName"
        auto alias = lvol_bdev.get_aliases().front();
        auto name = ::utils::StringUtils::substring_after_last_of(alias, '/');
        if (!name.empty()) {
            // TODO: validate against UUID
            IdentifierBuilder::set_uuid(volume, name);
        }
    }
    else {
        throw std::runtime_error("Could not read aliases of volume " + volume.get_name().value());
    }
}