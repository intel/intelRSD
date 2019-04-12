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
 * @file command/get_collection.cpp
 * @brief GetCollection sdv implementation
 * */

#include "agent-framework/module/common_components.hpp"
#include "agent-framework/command/registry.hpp"
#include "agent-framework/command/chassis_commands.hpp"

#include "ipmb/certificate_manager.hpp"
#include "base64/base64.hpp"

using namespace agent_framework::command;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::model::attribute;

namespace {

    using KeysVec = std::vector<std::string>;
    using Collections = attribute::Array<attribute::Collection>;

    void response_add_subcomponents(const KeysVec& keys,
            GetCollection::Response& response) {
        for (const auto& key : keys) {
            SubcomponentEntry s;
            s.set_subcomponent(key);
            response.add_entry(s);
        }
    }

    void process_manager(const Collection& collection, const std::string& uuid,
                         GetCollection::Response& response, const std::string& name) {
        if (enums::CollectionType::Chassis == collection.get_type()) {
            response_add_subcomponents(CommonComponents::get_instance()->
                get_chassis_manager().get_keys(uuid), response);
        }
        else if (enums::CollectionType::Managers == collection.get_type()) {
            response_add_subcomponents(CommonComponents::get_instance()->
                get_module_manager().get_keys(uuid), response);
        }
        else {
            THROW(agent_framework::exceptions::InvalidCollection, "chassis-agent",
                "Invalid collection name: \'" + name + "\'");
        }
    }

    const Collection find_collection(const Collections& collections,
                                     const std::string& name) {
        for (const auto& collection : collections) {
            if (name == collection.get_name()) {
                return collection;
            }
        }
        Collection c;
        c.set_type(enums::CollectionType::None);
        return c;
    }

    void get_collection(const GetCollection::Request& request, GetCollection::Response& response) {
        const auto& uuid = request.get_uuid();
        const auto& name = request.get_name();

        log_debug("chassis-agent", "GetCollection with parameters: component "
            << request.get_uuid() << ", name " << request.get_name());

        if (CommonComponents::get_instance()->get_module_manager().entry_exists(uuid)) {
            const Manager manager = CommonComponents::get_instance()->
                    get_module_manager().get_entry(uuid);
            const Collection collection =
                    find_collection(manager.get_collections(), name);

            process_manager(collection, uuid, response, name);
        }
        else {
            THROW(::agent_framework::exceptions::InvalidUuid, "chassis-agent",
                  "Component not found, invalid UUID: " + uuid);
        }
    }
}

REGISTER_COMMAND(GetCollection, ::get_collection);
