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
 * @file command/stubs/get_collection.cpp
 * @brief GetCollection stub implementation
 * */

#include "agent-framework/command/chassis/get_collection.hpp"
#include "agent-framework/module-ref/chassis_manager.hpp"
#include "agent-framework/logger_ext.hpp"

using namespace agent_framework::command;
using namespace agent_framework::model;
using namespace agent_framework::model::attribute;

class GetCollection : public chassis::GetCollection {
public:
    GetCollection() {}

    ~GetCollection();

    using KeysVec = std::vector<std::string>;
    using chassis::GetCollection::execute;
    using Collections = attribute::Array<attribute::Collection>;
    using ChassisComponents = agent_framework::module::ChassisManager;

    void execute(const Request& request, Response& response) {
        const auto& uuid = request.get_component();
        const auto& name = request.get_name();

        log_debug(GET_LOGGER("rpc"), "GetCollection with parameters: component "
                                     << request.get_component() << ", name " << request.get_name());

        if(ChassisComponents::get_instance()->
                get_module_manager().entry_exists(uuid)) {

            const Manager& manager = ChassisComponents::get_instance()->
                    get_module_manager().get_entry(uuid);
            const Collection& collection =
                    find_collection(manager.get_collections(), name);

            process_manager(collection, uuid, response, name);
        }
        else {
            THROW(::agent_framework::exceptions::InvalidUuid, "agent",
                  "Component not found, invalid uuid: " + uuid);
        }
    }

private:

    void process_manager(const Collection& collection, const std::string&
    uuid, Response& response, const std::string& name) {
        if(enums::CollectionType::Chassis == collection.get_type()) {
            response_add_subcomponents(ChassisComponents::get_instance()->
                get_chassis_manager().get_keys(uuid), response);
        }
        else if(enums::CollectionType::Managers == collection.get_type()) {
            response_add_subcomponents(ChassisComponents::get_instance()->
                    get_module_manager().get_keys(uuid), response);
        }
        else {
            THROW(::agent_framework::exceptions::InvalidParameters, "agent",
                  "Manager collection entry not found, invalid collection name: \'" + name + "\'");
        }
    }

    const Collection find_collection(const Collections& collections,
                                     const std::string& name) {
        for(const auto& collection : collections) {
            if(name == collection.get_name()) {
                return collection;
            }
        }
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
