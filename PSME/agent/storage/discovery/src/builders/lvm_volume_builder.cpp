/*!
 * @brief Volume builder class implementation.
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
 * @file lvm_volume_builder.cpp
 */

#include "agent-framework/discovery/builders/identifier_builder.hpp"
#include "discovery/builders/lvm_volume_builder.hpp"

#include "lvm/lvm_api.hpp"

using namespace agent::storage;
using namespace agent::storage::discovery;
using namespace agent_framework;


void LvmVolumeBuilder::update_with_lvm(model::Volume& volume,
                                       const ::storage::lvm::model::LogicalVolume& logical_volume) {

    volume.set_status(model::attribute::Status{
        model::enums::State::from_string(logical_volume.get_status()),
        model::enums::Health::from_string(logical_volume.get_health())
    });

    model::attribute::Capacity capacity{};
    capacity.set_allocated_bytes(logical_volume.get_capacity_b());

    capacity.set_thin_provisioned(logical_volume.is_snapshot());

    volume.set_capacity(capacity);

    volume.set_bootable(logical_volume.is_bootable());
    if (logical_volume.get_protection_status()) {
        volume.set_access_capabilities(model::Volume::AccessCapabilities{
            model::enums::AccessCapability::Read
        });
    }
    else {
        volume.set_access_capabilities(model::Volume::AccessCapabilities{
            model::enums::AccessCapability::Read,
            model::enums::AccessCapability::Write,
        });
    }
}

