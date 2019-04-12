/*!
 * @brief Storage subsystem builder class implementation.
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
 * @file storage_subsystem_builder.cpp
 */

#include "discovery/builders/storage_subsystem_builder.hpp"



using namespace agent::compute::discovery;
using namespace agent_framework::model;


agent_framework::model::StorageSubsystem StorageSubsystemBuilder::build_default(const std::string& parent_uuid) {
    StorageSubsystem storage{parent_uuid};
    storage.set_status({enums::State::Enabled, {}});

    storage.add_collection({enums::CollectionName::StorageControllers,
                            enums::CollectionType::StorageControllers
                           });

    storage.add_collection({enums::CollectionName::Drives,
                            enums::CollectionType::Drives
                           });

    return storage;
}
