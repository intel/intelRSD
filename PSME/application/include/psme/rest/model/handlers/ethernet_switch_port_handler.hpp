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
 * */

#pragma once



#include "agent-framework/module/requests/network/get_ethernet_switch_port_info.hpp"
#include "agent-framework/module/network_components.hpp"



namespace psme {
namespace rest {
namespace model {
namespace handler {

using EthernetSwitchPortHandlerBase = GenericHandler <
agent_framework::model::requests::GetEthernetSwitchPortInfo,
agent_framework::model::EthernetSwitchPort,
IdPolicy<agent_framework::model::enums::Component::EthernetSwitchPort, NumberingZone::PARENT_SPACE>>;

/*!
 * @brief EthernetSwitchPortHandler template specialization.
 *
 * This is necessary because we have to handle PortMembers and bound ACLs in
 * the way that is inconsistent with most other components.
 * */
class EthernetSwitchPortHandler : public EthernetSwitchPortHandlerBase {
public:
    EthernetSwitchPortHandler() : EthernetSwitchPortHandlerBase() {}


    virtual ~EthernetSwitchPortHandler();


protected:
    using NetworkComponents = agent_framework::module::NetworkComponents;


    /*!
     * @brief Specialization of fetch_siblings()
     *
     * An EthernetPort can be either a subcomponent of a EthernetSwitch, another EthernetPort or an ACL.
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
        if (Component::EthernetSwitch == ctx.get_parent_component()) {
            return EthernetSwitchPortHandlerBase::fetch_siblings(ctx, parent_uuid,
                                                                 collection_name);
        }
        else if (Component::EthernetSwitchPort == ctx.get_parent_component()) {
            fetch_parent_children(ctx, parent_uuid, collection_name,
                                  NetworkComponents::get_instance()->get_port_members_manager());
        }
        else if (Component::Acl == ctx.get_parent_component()) {
            fetch_parent_children(ctx, parent_uuid, collection_name,
                                  NetworkComponents::get_instance()->get_acl_port_manager());
        }
    }


    /*!
     * @brief  Specialization of remove_agent_data()
     *
     * This override is necessary to properly clean the Port <-> Port
     * and ACL <-> Port bindings for all the ports.
     *
     * @param[in] ctx keeps data that is required during processing and needs to be passed down to sub-handlers
     * @param[in] gami_id uuid of the agent whose data is to be removed.
     * */
    void remove_agent_data(Context& ctx, const std::string& gami_id) override {
        NetworkComponents::get_instance()->
            get_acl_port_manager().clean_resources_for_agent(gami_id);
        NetworkComponents::get_instance()->
            get_port_members_manager().clean_resources_for_agent(gami_id);
        EthernetSwitchPortHandlerBase::remove_agent_data(ctx, gami_id);
    }


    /*!
     * @brief  Specialization of do_remove() from GenericManager.
     *
     * This override is necessary for clearing the ACL <-> port bindings as
     * well as links to its PortMembers for given port.
     *
     * @param[in] ctx keeps data that is required during processing and needs to be passed down to sub-handlers
     * @param[in] uuid uuid of the port to be removed.
     * */
    void do_remove(Context& ctx, const std::string& uuid) override {
        auto& port_members_manager = NetworkComponents::get_instance()->
            get_port_members_manager();
        port_members_manager.remove_if([&uuid](const std::string& parent,
                                               const std::string& child, const std::string&) {
            return parent == uuid || child == uuid;
        });

        // port is the child in ACL <-> port relation
        NetworkComponents::get_instance()->
            get_acl_port_manager().remove_child(uuid);
        EthernetSwitchPortHandlerBase::do_remove(ctx, uuid);
    }

};


EthernetSwitchPortHandler::~EthernetSwitchPortHandler() {}

}
}
}
}

