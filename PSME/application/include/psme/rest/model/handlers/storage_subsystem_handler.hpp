/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 * @file storage_subsystem_handler.hpp
 *
 * Specialization of GenericHandler for StorageSubsystems
 * */

#pragma once
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/requests/common/get_storage_subsystem_info.hpp"

namespace psme {
namespace rest {
namespace model {
namespace handler {

using StorageSubsystemHandlerBase = GenericHandler<
agent_framework::model::requests::GetStorageSubsystemInfo,
agent_framework::model::StorageSubsystem,
IdPolicy<agent_framework::model::enums::Component::StorageSubsystem, NumberingZone::PARENT_SPACE>>;

/*!
 * @brief StorageSubsystemHandler template specialization
 *
 * Storage subsystems have collections of Drives.
 * Many-to-many-managers are used to store the logical connections between resources.
 * Above is the reason why we need separate implementation.
 */
class StorageSubsystemHandler : public StorageSubsystemHandlerBase {
public:
    StorageSubsystemHandler() : StorageSubsystemHandlerBase() {}
    virtual ~StorageSubsystemHandler();
protected:

    /*!
     * @brief Overrides remove_agent_data() from GenericManager
     *
     * This override is necessary to properly clean the contents of
     * StorageDrivesManager
     *
     * @param[in] ctx keeps data that is required during processing and needs to be passed down to sub-handlers
     * @param[in] gami_id uuid of the agent whose data is to be removed.
     * */
    void remove_agent_data(Context& ctx, const std::string& gami_id) override {
        StorageSubsystemHandlerBase::remove_agent_data(ctx, gami_id);

        agent_framework::module::CommonComponents::get_instance()->
            get_storage_subsystem_drives_manager().clean_resources_for_agent(gami_id);
    }

    /*!
     * @brief removes a component and all its descendants from the model
     *
     * @param[in] ctx keeps data that is required during processing and needs to be passed down to sub-handlers
     * @param uuid component's uuid
     * */
    void do_remove(Context& ctx, const std::string &uuid) override {
        agent_framework::module::CommonComponents::get_instance()->
            get_storage_subsystem_drives_manager().remove_parent(uuid);

        StorageSubsystemHandlerBase::do_remove(ctx, uuid);
    }
};

StorageSubsystemHandler::~StorageSubsystemHandler() {}

}
}
}
}

