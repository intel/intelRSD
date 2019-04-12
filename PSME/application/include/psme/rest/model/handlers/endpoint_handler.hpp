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
 * @file endpoint_handler.hpp
 *
 * Specialization of GenericHandler for Endpoint
 * */

#pragma once



#include "agent-framework/module/requests/common/get_endpoint_info.hpp"
#include "agent-framework/module/common_components.hpp"



namespace psme {
namespace rest {
namespace model {
namespace handler {

using EndpointHandlerBase = GenericHandler <
agent_framework::model::requests::GetEndpointInfo,
agent_framework::model::Endpoint,
IdPolicy<agent_framework::model::enums::Component::Endpoint, NumberingZone::PARENT_SPACE>>;

/*!
 * @brief EndpointHandler template specialization.
 *
 * This is necessary because we have to handle Drives assigned to StorageSubsystems in
 * the way that is inconsistent with most other components.
 * */
class EndpointHandler : public EndpointHandlerBase {
public:
    EndpointHandler() : EndpointHandlerBase() {}


    virtual ~EndpointHandler();


protected:
    using PncComponents = agent_framework::module::PncComponents;
    using CommonComponents = agent_framework::module::CommonComponents;

    /*!
     * @brief collection might be strong or weak
     *
     * @param[in] parent_component Component of parent node
     */
    bool is_strong_collection(const Component parent_component) {
        return (Component::Fabric == parent_component);
    }


    /*!
     * @brief Specialization of fetch_siblings()
     *
     * An Drive can be either a subcomponent of a Chassis, or a StorageSubsystem.
     * This code handles all three cases.
     *
     * @param[in] ctx keeps data that is required during processing and
     *                needs to be passed down to sub-handlers
     * @param[in] parent_uuid uuid of parent whose subcomponents we want to
     *                        retrieve
     * @param[in] collection_name name of collection of elements to fetch
     */
    void fetch_siblings(Context& ctx, const std::string& parent_uuid,
                        const std::string& collection_name) override {
        if (is_strong_collection(ctx.get_parent_component())) {
            return EndpointHandlerBase::fetch_siblings(ctx, parent_uuid, collection_name);
        }
        else if (Component::Zone == ctx.get_parent_component()) {
            fetch_parent_children(ctx, parent_uuid, collection_name,
                                  CommonComponents::get_instance()->get_zone_endpoint_manager());
        }
        else {
            log_debug("rest", ctx.indent
                << "[" << static_cast<char>(ctx.mode) << "] "
                << "Found a Drive under an unexpected parent " << parent_uuid);
        }
    }


    /*!
     * @brief  Specialization of remove_agent_data()
     *
     * This override is necessary to properly clean the Endpoint <-> Port and Zone <-> Endpoint
     * bindings for all the Endpoints.
     *
     * @param[in] ctx keeps data that is required during processing and needs to be passed down to sub-handlers
     * @param[in] gami_id uuid of the agent whose data is to be removed.
     * */
    void remove_agent_data(Context& ctx, const std::string& gami_id) override {
        PncComponents::get_instance()->
            get_endpoint_port_manager().clean_resources_for_agent(gami_id);
        CommonComponents::get_instance()->
            get_zone_endpoint_manager().clean_resources_for_agent(gami_id);
        EndpointHandlerBase::remove_agent_data(ctx, gami_id);
    }


    /*!
     * @brief  Specialization of do_remove() from GenericManager.
     *
     * This override is necessary for clearing the Endpoint <-> Port and Zone <-> Endpoint bindings.
     *
     * @param[in] ctx keeps data that is required during processing and needs to be passed down to sub-handlers
     * @param[in] uuid uuid of the endpoint to be removed.
     * */
    void do_remove(Context& ctx, const std::string& uuid) override {
        // endpoint is the parent in Endpoint <-> Port relation
        PncComponents::get_instance()->
            get_endpoint_port_manager().remove_parent(uuid);

        // endpoint is the child in Zone <-> Endpoint relation
        CommonComponents::get_instance()->
            get_zone_endpoint_manager().remove_child(uuid);
        EndpointHandlerBase::do_remove(ctx, uuid);
    }


    bool do_accept_recursively(ResourceVisitor& visitor,
                               const std::string& parent_uuid,
                               const Component parent_component) override {
        if (is_strong_collection(parent_component)) {
            return EndpointHandlerBase::do_accept_recursively(visitor, parent_uuid, parent_component);
        }
        else {
            auto& manager = CommonComponents::get_instance()->get_zone_endpoint_manager();
            auto children = manager.get_children(parent_uuid);
            for (const std::string& child_uuid : children) {
                if (!do_accept(visitor, child_uuid)) {
                    return false; // break
                }
            }
        }
        return true;
    }
};


EndpointHandler::~EndpointHandler() {}

}
}
}
}
