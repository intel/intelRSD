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
 *
 * @file command/stubs/get_collection.cpp
 * @brief Implementation of GetCollection command
 * */

#include "agent-framework/exceptions/exception.hpp"
#include "agent-framework/module-ref/enum/common.hpp"
#include "agent-framework/module-ref/constants/common.hpp"
#include "agent-framework/module-ref/network_manager.hpp"
#include "agent-framework/command/network/get_collection.hpp"
#include "agent-framework/logger_ext.hpp"

using namespace agent_framework::command;
using namespace agent_framework::model;
using namespace agent_framework::model::attribute;

/*! @brief GetCollection implementation */
class GetCollection : public network::GetCollection {
public:
    GetCollection() {}

    ~GetCollection();

    using KeysVec = std::vector<std::string>;
    using network::GetCollection::execute;
    using Collections = attribute::Array<attribute::Collection>;
    using NetworkManager = agent_framework::module::NetworkManager;

    void execute(const Request& request, Response& response) {
        const auto& uuid = request.get_component();
        const auto& name = request.get_name();

        if (NetworkManager::get_instance()->
            get_module_manager().entry_exists(uuid)) {

            const Manager& manager = NetworkManager::get_instance()->
                get_module_manager().get_entry(uuid);
            const Collection& collection =
                find_collection(manager.get_collections(), name);

            process_manager(collection, uuid, response, name);
        }
        else if (NetworkManager::get_instance()->
            get_switch_manager().entry_exists(uuid)) {

            const Switch& sw = NetworkManager::get_instance()->
                get_switch_manager().get_entry(uuid);
            const Collection& collection =
                find_collection(sw.get_collections(), name);

            process_switch(collection, uuid, response, name);
        }
        else if (NetworkManager::get_instance()->
            get_port_manager().entry_exists(uuid)) {

            const SwitchPort& port = NetworkManager::get_instance()->
                get_port_manager().get_entry(uuid);
            const Collection& collection =
                find_collection(port.get_collections(), name);

            process_port(collection, uuid, response, name);
        }
        else if (NetworkManager::get_instance()->
            get_acl_manager().entry_exists(uuid)) {

            const Acl& acl = NetworkManager::get_instance()->
                get_acl_manager().get_entry(uuid);
            const Collection& collection =
                find_collection(acl.get_collections(), name);

            process_acl(collection, uuid, response, name);
        }
        else if (NetworkManager::get_instance()->
                get_port_vlan_manager().entry_exists(uuid)
            || NetworkManager::get_instance()->
                get_vlan_manager().entry_exists(uuid)
            || NetworkManager::get_instance()->
                get_remote_switch_manager().entry_exists(uuid)) {

            THROW(::agent_framework::exceptions::InvalidUuid, "network-agent",
                  "Component does not have collections.");
        }
        else {
            THROW(::agent_framework::exceptions::InvalidUuid, "network-agent",
                  "Component not found, invalid uuid: '" + uuid + "'.");
        }

        log_debug(GET_LOGGER("network-agent"),
                 "Getting collection successful.");
    }

private:

    void process_manager(const Collection& collection, const std::string&
        uuid, Response& response, const std::string& name) {
        // Two possible collection types of a manager
        if (enums::CollectionType::EthernetSwitches ==
            collection.get_type()) {
            response_add_subcomponents(NetworkManager::get_instance()->
                get_switch_manager().get_keys(uuid), response);
        }
        else if (enums::CollectionType::Chassis == collection.get_type()) {
            response_add_subcomponents(NetworkManager::get_instance()->
                get_chassis_manager().get_keys(uuid), response);
        }
        else {
            THROW(::agent_framework::exceptions::InvalidParameters,
                  "network-agent",
                  "No collection named \'" + name +
                  "\' for Manager " + uuid + "!");
        }
    }

    void process_switch(const Collection& collection, const std::string& uuid,
        Response& response, const std::string& name) {
        // Three possible collection types of a switch
        if (enums::CollectionType::Ports == collection.get_type()) {
            response_add_subcomponents(NetworkManager::get_instance()->
                get_port_manager().get_keys(uuid), response);
        }
        else if (enums::CollectionType::Vlans == collection.get_type()) {
            response_add_subcomponents(NetworkManager::get_instance()->
                get_vlan_manager().get_keys(uuid), response);
        }
        else if (enums::CollectionType::NeighborSwitches ==
            collection.get_type()) {
            response_add_subcomponents(NetworkManager::get_instance()->
                get_remote_switch_manager().get_keys(uuid), response);
        }
        else if (enums::CollectionType::Acls == collection.get_type()) {
            response_add_subcomponents(NetworkManager::get_instance()->
                get_acl_manager().get_keys(uuid), response);
        }
        else {
            THROW(::agent_framework::exceptions::InvalidParameters,
                  "network-agent",
                  "No collection named \'" + name +
                  "\' for Ethernet Switch " + uuid + "!");
        }
    }

    void process_port(const Collection& collection, const std::string& uuid,
        Response& response, const std::string& name) {
        // Four possible collection types of a port
        if (enums::CollectionType::PortVlans == collection.get_type()) {
            response_add_subcomponents(NetworkManager::get_instance()->
                get_port_vlan_manager().get_keys(uuid), response);
        }
        else if (enums::CollectionType::PortMembers ==
            collection.get_type()) {
            response_add_subcomponents(NetworkManager::get_instance()->
                get_port_members_manager().get_children(uuid), response);
        }
        else if (enums::CollectionType::Acls == collection.get_type()) {
            // ACLs are always the parents in ACL <-> port relation
            response_add_subcomponents(NetworkManager::get_instance()->
                get_acl_port_manager().get_parents(uuid), response);
        }
        else if (enums::CollectionType::StaticMacs == collection.get_type()) {
            response_add_subcomponents(NetworkManager::get_instance()->
                    get_static_mac_manager().get_keys(uuid), response);
        }
        else {
            THROW(::agent_framework::exceptions::InvalidParameters, "agent",
                "No collection named \'" + name +
                "\' for Ethernet Switch Port " + uuid + "!");
        }
    }

    void process_acl(const Collection& collection, const std::string& uuid,
        Response& response, const std::string& name) {
        // Two possible collection types of a port
        if (enums::CollectionType::Rules == collection.get_type()) {
            response_add_subcomponents(NetworkManager::get_instance()->
                get_acl_rule_manager().get_keys(uuid), response);
        }
        else if (enums::CollectionType::Ports == collection.get_type()) {
            // ports are always the children in ACL <-> port relation
            response_add_subcomponents(NetworkManager::get_instance()->
                get_acl_port_manager().get_children(uuid), response);
        }
        else {
            THROW(::agent_framework::exceptions::InvalidParameters, "agent",
                "No collection named \'" + name +
                "\' for ACL " + uuid + "!");
        }
    }



    const Collection find_collection(const Collections& collections,
        const std::string& name) {
        for (const auto& collection : collections) {
            if (name == collection.get_name()) {
                return collection;
            }
        }
        // Returning a non-existent collection
        Collection c;
        c.set_type(enums::CollectionType::None);
        return c;
    }

    void response_add_subcomponents(const KeysVec& keys, Response& response) {
        for (const auto& key : keys) {
            SubcomponentEntry s;
            s.set_subcomponent(key);
            response.add_subcomponent(s);
        }
    }
};

GetCollection::~GetCollection() {}

static Command::Register<GetCollection> g("Stubs");
