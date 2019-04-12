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
 * @file drive_handler.hpp
 *
 * Specialization of GenericHandler for Drive
 * */

#pragma once



#include "agent-framework/module/requests/common/get_drive_info.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/pnc_components.hpp"



namespace psme {
namespace rest {
namespace model {
namespace handler {

using DriveHandlerBase = GenericHandler<
    agent_framework::model::requests::GetDriveInfo,
    agent_framework::model::Drive,
    IdPolicy<agent_framework::model::enums::Component::Drive, NumberingZone::PARENT_SPACE>>;

/*!
 * @brief DriveHandler template specialization.
 *
 * This is necessary because we have to handle Drives assigned to StorageSubsystems in
 * the way that is inconsistent with most other components.
 * */
class DriveHandler : public DriveHandlerBase {
public:

    DriveHandler() : DriveHandlerBase() {}

    virtual ~DriveHandler();


protected:
    using StorageSubsystem = agent_framework::model::StorageSubsystem;
    using StorageService = agent_framework::model::StorageService;
    using Drive = agent_framework::model::Drive;
    using PcieFunction = agent_framework::model::PcieFunction;

    /*!
     * @brief collection might be strong or weak
     *
     * @param[in] parent_component Component of parent node
     */
    bool is_strong_collection(const Component parent_component) {
        return (Component::Chassis == parent_component);
    }


    /*!
     * @brief Specialization of fetch_siblings()
     *
     * An Drive can be either a subcomponent of a Chassis, StorageService, or a StorageSubsystem.
     * This code handles all three cases.
     *
     * @param[in] ctx keeps data that is required during processing and
     *                needs to be passed down to sub-handlers
     * @param[in] parent_uuid uuid of parent whose subcomponents we want to
     *                        retrieve
     * @param[in] collection_name name of collection of elements to fetch
     */
    void fetch_siblings(Context& ctx, const Uuid& parent_uuid, const std::string& collection_name) override {
        if (is_strong_collection(ctx.get_parent_component())) {
            return DriveHandlerBase::fetch_siblings(ctx, parent_uuid, collection_name);
        }
        else if (Component::StorageSubsystem == ctx.get_parent_component()) {
            fetch_parent_children(ctx, parent_uuid, collection_name,
                                  agent_framework::module::get_m2m_manager<StorageSubsystem, Drive>());
        }
        else if (Component::StorageService == ctx.get_parent_component()) {
            fetch_parent_children(ctx, parent_uuid, collection_name,
                                  agent_framework::module::get_m2m_manager<StorageService, Drive>());
        }
        else {
            log_debug("rest", ctx.indent << "[" << static_cast<char>(ctx.mode) << "] "
                << "Found a Drive under an unexpected parent " << parent_uuid);
        }
    }


    /*!
     * @brief Specialization of remove_agent_data()
     *
     * This override is necessary to properly clean the StorageSubsystem/StorageService <-> Drive and
     * Drive <-> PCIeFunction bindings for all the Drives.
     *
     * @param[in] ctx keeps data that is required during processing and needs to be passed down to sub-handlers
     * @param[in] gami_id uuid of the agent whose data is to be removed.
     * */
    void remove_agent_data(Context& ctx, const std::string& gami_id) override {
        agent_framework::module::get_m2m_manager<StorageSubsystem, Drive>().clean_resources_for_agent(gami_id);
        agent_framework::module::get_m2m_manager<StorageService, Drive>().clean_resources_for_agent(gami_id);
        agent_framework::module::get_m2m_manager<Drive, PcieFunction>().clean_resources_for_agent(gami_id);
        DriveHandlerBase::remove_agent_data(ctx, gami_id);
    }


    /*!
     * @brief Specialization of do_remove() from GenericManager.
     *
     * This override is necessary for clearing the StorageSubsystem/StorageService <-> Drive and
     * Drive <-> PCIeFunction bindings.
     *
     * @param[in] ctx keeps data that is required during processing and needs to be passed down to sub-handlers
     * @param[in] uuid uuid of the drive to be removed.
     * */
    void do_remove(Context& ctx, const Uuid& uuid) override {
        // drive is the child in StorageSubsystem <-> Drive relation
        agent_framework::module::get_m2m_manager<StorageSubsystem, Drive>().remove_child(uuid);
        // drive is the child in StorageService <-> Drive relation
        agent_framework::module::get_m2m_manager<StorageService, Drive>().remove_child(uuid);

        // drive is the parent in Drive <-> PCIeFunction relation
        agent_framework::module::get_m2m_manager<Drive, PcieFunction>().remove_parent(uuid);

        DriveHandlerBase::do_remove(ctx, uuid);
    }


    bool do_accept_recursively(ResourceVisitor& visitor,
                               const Uuid& parent_uuid,
                               const Component parent_component) override {
        using namespace agent_framework::module;
        if (is_strong_collection(parent_component)) {
            return DriveHandlerBase::do_accept_recursively(visitor, parent_uuid, parent_component);
        }
        else {
            auto do_accept_for_children = [this, &visitor, &parent_uuid](managers::ManyToManyManager& manager) {
                auto children = manager.get_children(parent_uuid);
                for (const auto& child_uuid : children) {
                    if (!do_accept(visitor, child_uuid)) {
                        return false; // break
                    }
                }
                return true;
            };

            if (Component::StorageSubsystem == parent_component) {
                auto& manager = get_m2m_manager<StorageSubsystem, Drive>();
                return do_accept_for_children(manager);
            }
            else if (Component::StorageService == parent_component) {
                auto& manager = get_m2m_manager<StorageService, Drive>();
                return do_accept_for_children(manager);
            }
        }
        return false;
    }
};


DriveHandler::~DriveHandler() {}

}
}
}
}
