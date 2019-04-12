/*!
 * @brief Manager builder class implementation.
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
 * @file manager_builder.cpp
 */

#include "agent-framework/discovery/builders/manager_builder.hpp"



using namespace agent_framework::discovery;


agent_framework::model::Manager ManagerBuilder::build_enclosure() {
    model::Manager manager{};

    manager.set_manager_type(model::enums::ManagerInfoType::EnclosureManager);
    manager.set_status({model::enums::State::Enabled, model::enums::Health::OK});

    manager.add_collection(model::attribute::Collection{
        model::enums::CollectionName::StorageServices,
        model::enums::CollectionType::StorageServices
    });
    manager.add_collection(model::attribute::Collection{
        model::enums::CollectionName::Chassis,
        model::enums::CollectionType::Chassis
    });
    manager.add_collection(model::attribute::Collection{
        model::enums::CollectionName::Systems,
        model::enums::CollectionType::Systems
    });
    manager.add_collection(model::attribute::Collection{
        model::enums::CollectionName::Fabrics,
        model::enums::CollectionType::Fabrics
    });

    return manager;
}
