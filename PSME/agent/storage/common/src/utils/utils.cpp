/*!
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
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
 * @file utils.cpp
 * @brief Utility functions
 */

#include "agent-framework/exceptions/exception.hpp"
#include "agent-framework/module/storage_components.hpp"

#include "agent/utils/utils.hpp"



using namespace agent_framework::module;
using namespace agent_framework::model;


void agent::storage::utils::lock_volume_for_initialization(const Uuid& uuid) {
    auto volume_ref = get_manager<Volume>().get_entry_reference(uuid);
    if (volume_ref->get_is_being_initialized()) {
        throw std::runtime_error("Volume is already being initialized!");
    }
    else {
        volume_ref->set_is_being_initialized(true);
        volume_ref->set_status(attribute::Status{enums::State::Starting, volume_ref->get_status().get_health()});
    }
}


void agent::storage::utils::unlock_volume_for_initialization(const Uuid& uuid, bool success) {
    auto volume_ref = get_manager<Volume>().get_entry_reference(uuid);
    volume_ref->set_is_being_initialized(false);
    if (!success) {
        volume_ref->set_status(attribute::Status{enums::State::Enabled, enums::Health::Warning});
    }
}

void agent::storage::utils::update_storage_pool_relations(const Uuid& old_uuid, const Uuid& new_uuid) {
    get_m2m_manager<StoragePool, Volume>().update_parent(old_uuid, new_uuid);
}
