/*!
 * @brief Storage Service builder class implementation.
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
 * @file storage_service_builder.cpp
 */

#include "agent-framework/discovery/builders/storage_service_builder.hpp"



using namespace agent_framework::discovery;


agent_framework::model::StorageService StorageServiceBuilder::build_default(const Uuid& parent_uuid) {
    model::StorageService storage_service{parent_uuid};
    storage_service.set_status({model::enums::State::Enabled, model::enums::Health::OK});

    storage_service.add_collection(model::attribute::Collection{
        model::enums::CollectionName::StoragePools,
        model::enums::CollectionType::StoragePools
    });
    storage_service.add_collection(model::attribute::Collection{
        model::enums::CollectionName::Volumes,
        model::enums::CollectionType::Volumes
    });
    storage_service.add_collection(model::attribute::Collection{
        model::enums::CollectionName::Drives,
        model::enums::CollectionType::Drives
    });

    return storage_service;
}
