/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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

#pragma once
#include "agent-framework/module/storage_components.hpp"
#include "agent-framework/module/requests/storage/get_logical_drive_info.hpp"

namespace psme {
namespace rest {
namespace model {
namespace handler {

using LogicalDriveHandlerBase = GenericHandler<
        agent_framework::model::requests::GetLogicalDriveInfo,
        agent_framework::model::LogicalDrive,
        IdPolicy<agent_framework::model::enums::Component::LogicalDrive, NumberingZone::PARENT_SPACE>>;

/*!
 * @brief LogicalDriveHandler template specialization
 *
 * Logical drives come from two sources
 * - elements of sub-collection of StorageServices
 * - elements of sub-collection of Logical drives (recursive definition)
 * Above is the reason why we need separate implementation.
 */
class LogicalDriveHandler : public LogicalDriveHandlerBase {
public:
    LogicalDriveHandler() : LogicalDriveHandlerBase() {}
    virtual ~LogicalDriveHandler();
protected:
    using StorageComponents = agent_framework::module::StorageComponents;


    /*!
     * @brief collection might be strong or weak
     *
     * @param[in] parent_component Component of parent node
     */
    bool is_strong_collection(const Component parent_component) {
        return (Component::StorageService == parent_component);
    }

    /*!
     * @brief Specialization of fetch siblings
     * @param ctx - keeps data that is required during processing and need to be passed down to sub-handlers
     * @param parent_uuid - uuid of parent for components we want to retrieve
     * @param collection_name name of the collection
     * Logical Drive can be either sub-component of StorageService or sub-component of another LogicalDrive
     * This code handles both cases
     */
    void fetch_siblings(Context& ctx, const std::string &parent_uuid, const std::string &collection_name) override {
        if (is_strong_collection(ctx.get_parent_component())) {
            return LogicalDriveHandlerBase::fetch_siblings(ctx, parent_uuid, collection_name);
        }
        else if (Component::LogicalDrive == ctx.get_parent_component()) {
            fetch_parent_children(ctx, parent_uuid, collection_name,
                                  StorageComponents::get_instance()->get_logical_drive_members_manager());
        }
    }

    /*!
     * @brief Overrides remove_agent_data() from GenericManager
     *
     * @param[in] ctx keeps data that is required during processing and needs to be passed down to sub-handlers
     * This override is necessary to properly clean the contents of
     * LogicalDrivesMembersManager
     * */
    void remove_agent_data(Context& ctx, const std::string& gami_id) override {
        LogicalDriveHandlerBase::remove_agent_data(ctx, gami_id);

        StorageComponents::get_instance()->
            get_logical_drive_members_manager().clean_resources_for_agent(gami_id);

        StorageComponents::get_instance()->
            get_physical_drive_members_manager().clean_resources_for_agent(gami_id);
    }

    /*!
     * @brief removes a component and all its descendants from the model
     *
     * @param ctx keeps data that is required during processing and needs to be passed down to sub-handlers
     * @param uuid component's uuid
     * */
    void do_remove(Context& ctx, const std::string &uuid) override {
        // remove links where logical to remove is either parent or child of other logical
        auto& logical_drive_members_manager = agent_framework::module::StorageComponents::get_instance()->get_logical_drive_members_manager();
        logical_drive_members_manager.remove_if([&uuid](const std::string& parent, const std::string& child, const std::string&) {
            return parent == uuid || child == uuid;
        });

        // remove links where logical to remove is parent of physical
        StorageComponents::get_instance()->get_physical_drive_members_manager().remove_parent(uuid);

        LogicalDriveHandlerBase::do_remove(ctx, uuid);
    }

    bool do_accept_recursively(ResourceVisitor& visitor,
                               const std::string& parent_uuid,
                               const Component parent_component) override {
        if (is_strong_collection(parent_component)) {
            return LogicalDriveHandlerBase::do_accept_recursively(visitor, parent_uuid, parent_component);
        }
        else if (Component::LogicalDrive == parent_component) {
            auto& manager = StorageComponents::get_instance()->get_logical_drive_members_manager();
            auto children = manager.get_children(parent_uuid);
            for (const std::string& child_uuid : children) {
                if (!do_accept(visitor, child_uuid)) {
                    return false; //break
                }
            }
        }
        return true;
    }
};

LogicalDriveHandler::~LogicalDriveHandler() {}

}
}
}
}

