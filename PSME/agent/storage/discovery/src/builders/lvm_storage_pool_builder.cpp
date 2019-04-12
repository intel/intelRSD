/*!
 * @brief LVM Storage pool builder class implementation.
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
 * @file lvm_storage_pool_builder.cpp
 */

#include "lvm/model/volume_group.hpp"
#include "discovery/builders/lvm_storage_pool_builder.hpp"

using namespace agent::storage;
using namespace agent::storage::discovery;
using namespace agent_framework;


void LvmStoragePoolBuilder::update_with_lvm(model::StoragePool& storage_pool,
                                            const ::storage::lvm::model::VolumeGroup& volume_group) {

    storage_pool.set_status(model::attribute::Status{
        model::enums::State::from_string(volume_group.get_status()),
        model::enums::Health::from_string(volume_group.get_health())
    });

    model::attribute::Capacity capacity{};
    capacity.set_allocated_bytes(volume_group.get_capacity_b());
    capacity.set_consumed_bytes(volume_group.get_capacity_b() - volume_group.get_free_b());
    capacity.set_guaranteed_bytes(volume_group.get_free_b());
    storage_pool.set_capacity(capacity);
}

