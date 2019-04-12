/*!
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
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
 * @file storage_service_handler.hpp
 * @brief Specialization of GenericHandler for Storage Service
 * */

#pragma once



#include "agent-framework/module/requests/storage/get_storage_service_info.hpp"
#include "agent-framework/module/common_components.hpp"



namespace psme {
namespace rest {
namespace model {
namespace handler {

using StorageServiceHandlerBase = GenericHandler<
    agent_framework::model::requests::GetStorageServiceInfo,
    agent_framework::model::StorageService,
    IdPolicy<agent_framework::model::enums::Component::StorageService, NumberingZone::SHARED>>;

/*!
 * @brief StorageServiceHandler template specialization.
 *
 * This is necessary because we have to handle Systems assigned to StorageServices in
 * the way that is inconsistent with most other components.
 * */
class StorageServiceHandler : public StorageServiceHandlerBase {
public:

    StorageServiceHandler() : StorageServiceHandlerBase() {}

    virtual ~StorageServiceHandler();


protected:
    using StorageService = agent_framework::model::StorageService;
    using System = agent_framework::model::System;

    /*!
     * @brief Collection might be strong or weak.
     * @param[in] parent_component Component of parent node.
     * @return True if collection is strong.
     */
    bool is_strong_collection(const Component parent_component) {
        return (Component::Manager == parent_component);
    }


    /*!
     * @brief Specialization of fetch_siblings()
     *
     * A Volume can be either a subcomponent of a StorageService, or a StoragePool.
     * This code handles all two cases.
     *
     * @param[in] ctx Keeps data that is required during processing and
     *                needs to be passed down to sub-handlers.
     * @param[in] parent_uuid UUID of parent whose subcomponents we want to retrieve.
     * @param[in] collection_name Name of collection of elements to fetch.
     */
    void fetch_siblings(Context& ctx, const Uuid& parent_uuid,
                        const std::string& collection_name) override {
        if (is_strong_collection(ctx.get_parent_component())) {
            return StorageServiceHandlerBase::fetch_siblings(ctx, parent_uuid, collection_name);
        }
        else if (Component::System == ctx.get_parent_component()) {
            fetch_parent_children(ctx, parent_uuid, collection_name,
                                  agent_framework::module::get_m2m_manager<System, StorageService>());
        }
        else {
            log_warning("rest", ctx.indent << "[" << static_cast<char>(ctx.mode) << "] "
                << "Found a Storage Service under an unexpected parent " << parent_uuid);
        }
    }


    /*!
     * @brief Specialization of remove_agent_data()
     *
     * This override is necessary to properly clean the bindings for all the Storage Services.
     *
     * @param[in] ctx Keeps data that is required during processing and needs to be passed down to sub-handlers.
     * @param[in] gami_id UUID of the agent whose data is to be removed.
     * */
    void remove_agent_data(Context& ctx, const std::string& gami_id) override {
        agent_framework::module::get_m2m_manager<System, StorageService>().clean_resources_for_agent(gami_id);
        StorageServiceHandlerBase::remove_agent_data(ctx, gami_id);
    }


    /*!
     * @brief Specialization of do_remove() from GenericManager.
     *
     * This override is necessary for clearing the bindings.
     *
     * @param[in] ctx Keeps data that is required during processing and needs to be passed down to sub-handlers.
     * @param[in] uuid UUID of the volume to be removed.
     * */
    void do_remove(Context& ctx, const Uuid& uuid) override {
        agent_framework::module::get_m2m_manager<System, StorageService>().remove_child(uuid);
        StorageServiceHandlerBase::do_remove(ctx, uuid);
    }


    bool do_accept_recursively(ResourceVisitor& visitor, const Uuid& parent_uuid,
                               const Component parent_component) override {
        if (is_strong_collection(parent_component)) {
            return StorageServiceHandlerBase::do_accept_recursively(visitor, parent_uuid, parent_component);
        }
        else {
            auto& manager = agent_framework::module::get_m2m_manager<System, StorageService>();
            auto children = manager.get_children(parent_uuid);

            for (const auto& child_uuid : children) {
                if (!do_accept(visitor, child_uuid)) {
                    return false; // break
                }
            }
        }
        return true;
    }
};


StorageServiceHandler::~StorageServiceHandler() {}

}
}
}
}
