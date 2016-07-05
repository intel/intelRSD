/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
#include "agent-framework/module-ref/requests/network/get_ethernet_switch_port_info.hpp"
#include "agent-framework/module-ref/network_manager.hpp"
#include "agent-framework/module-ref/model/switch_port.hpp"
#include "psme/rest-ref/model/handlers/handler_helpers.hpp"

namespace psme {
namespace rest {
namespace model {
namespace handler {

using SwitchPortHandlerBase = GenericHandler<
        agent_framework::model::requests::GetEthernetSwitchPortInfo,
        agent_framework::model::SwitchPort, LocalIdPolicy>;

/*!
 * @brief SwitchPortHandler template specialization.
 *
 * This is necessary because we have to handle PortMembers and bound ACLs in
 * the way that is inconsistent with most other components.
 * */
class SwitchPortHandler : public SwitchPortHandlerBase {
public:
    SwitchPortHandler() : SwitchPortHandlerBase() {}
    virtual ~SwitchPortHandler();

protected:
    using NetworkManager = agent_framework::module::NetworkManager;

    /*!
     * @brief Specialization of fetch_siblings()
     *
     * An ACL can be either a subcomponent of a Switch or an ACL.
     * This code handles both cases.
     *
     * @param[in] ctx - keeps data that is required during processing and
     * needs to be passed down to sub-handlers
     * @param[in] parent_uuid - uuid of parent whose subcomponents we want to
     * retrieve
     * @param[in] collection_name name of the collection
     */
    void fetch_siblings(Context& ctx, const std::string& parent_uuid,
        const std::string& collection_name) override {
        if (Component::Switch == ctx.get_parent_component()) {
            return SwitchPortHandlerBase::fetch_siblings(ctx, parent_uuid,
                collection_name);
        }
        else if (Component::SwitchPort == ctx.get_parent_component()) {
            fetch_port_members(ctx, parent_uuid, collection_name);
        }
        else if (Component::Acl == ctx.get_parent_component()) {
            fetch_acl_ports(ctx, parent_uuid, collection_name);
        }
    }

    /*!
     * @brief  Specialization of remove_agent_data()
     *
     * This override is necessary to properly clean the ACL <-> port
     * bindings for all the ports.
     *
     * @param[in] gami_id uuid of the agent whose data is to be removed.
     * */
    void remove_agent_data(const std::string& gami_id) override {
        NetworkManager::get_instance()->
            get_acl_port_manager().clean_resources_for_agent(gami_id);
        NetworkManager::get_instance()->
            get_port_members_manager().clean_resources_for_agent(gami_id);
        SwitchPortHandlerBase::remove_agent_data(gami_id);
    }

    /*!
     * @brief  Specialization ofs remove() from GenericManager.
     *
     * This override is necessary for clearing the ACL <-> port bindings as
     * well as links to its PortMembers for given port.
     *
     * @param[in] uuid uuid of the port to be removed.
     * */
    void remove(const std::string& uuid) override {
        auto& port_members_manager = NetworkManager::get_instance()->
            get_port_members_manager();
        port_members_manager.remove_if([&uuid](const std::string& parent,
            const std::string& child, const std::string&) {
            return parent == uuid || child == uuid;
        });

        // port is the child in ACL <-> port relation
        NetworkManager::get_instance()->
            get_acl_port_manager().remove_child(uuid);
        SwitchPortHandlerBase::remove(uuid);
    }

    /*!
     * @brief Fetches port members for given Switch Port
     *
     * @param[in] ctx State of the handler passed down when handling request
     * @param[in] parent_uuid uuid of parent node
     */
    void fetch_port_members(Context& ctx, const std::string& parent_uuid, const
         std::string& collection_name) {
        log_debug(GET_LOGGER("rest"), ctx.indent
            << "[" << static_cast<char>(ctx.mode) << "] "
            << "Fetching PortMembers for parent=" << parent_uuid);

        const auto fetched = fetch_sibling_uuid_list(ctx, parent_uuid,
            collection_name);
        std::vector<std::string> to_add, to_remove;
        auto& port_members_manager = NetworkManager::get_instance()->
            get_port_members_manager();
        helpers::what_to_add_what_to_remove(fetched,
                                            parent_uuid,
                                            port_members_manager,
                                            to_add,
                                            to_remove);


        for (const auto& uuid : to_add) {
            log_debug(GET_LOGGER("rest"), ctx.indent
                << "[" << static_cast<char>(ctx.mode) << "] "
                << "Adding PortMember " << uuid
                << " to port " << parent_uuid);
            port_members_manager.add_entry(parent_uuid, uuid,
                ctx.agent->get_gami_id());
        }

        for (const auto& uuid : to_remove) {
            log_debug(GET_LOGGER("rest"), ctx.indent
                << "[" << static_cast<char>(ctx.mode) << "] "
                << "Removing PortMember " << uuid
                << " from port " << parent_uuid);
            port_members_manager.remove_entry(parent_uuid, uuid);
        }
    }

    /*!
     * @brief Fetches ports for an ACL
     *
     * @param[in] ctx State of the handler passed down when handling request
     * @param[in] parent_uuid uuid of parent node
     */
    void fetch_acl_ports(Context& ctx, const std::string& parent_uuid, const
        std::string& collection_name) {
        log_debug(GET_LOGGER("rest"), ctx.indent
              << "[" << static_cast<char>(ctx.mode) << "] "
              << "Binding ports to ACL " << parent_uuid);

        const auto fetched = fetch_sibling_uuid_list(ctx, parent_uuid,
                                               collection_name);
        std::vector<std::string> to_add, to_remove;
        auto& manager = NetworkManager::get_instance()->
            get_acl_port_manager();

        helpers::what_to_add_what_to_remove(fetched,
                                            parent_uuid,
                                            manager,
                                            to_add,
                                            to_remove);

        for (const auto& uuid : to_add) {
            log_debug(GET_LOGGER("rest"), ctx.indent
                  << "[" << static_cast<char>(ctx.mode) << "] "
                  << "Binding port " << uuid << " to ACL "
                  << parent_uuid);
            manager.add_entry(parent_uuid, uuid, ctx.agent->get_gami_id());
        }

        for (const auto& uuid : to_remove) {
            log_debug(GET_LOGGER("rest"), ctx.indent
                  << "[" << static_cast<char>(ctx.mode) << "] "
                  << "Unbinding port " << uuid << " from ACL "
                  << parent_uuid);
            manager.remove_entry(parent_uuid, uuid);
        }
    }
};

SwitchPortHandler::~SwitchPortHandler() {}

}
}
}
}

