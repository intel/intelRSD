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
 * */

#include "psme/rest/constants/constants.hpp"
#include "psme/rest/endpoints/message_registry_file_collection.hpp"
#include "psme/rest/registries/managers/message_registry_file_manager.hpp"



using namespace psme::rest;
using namespace psme::rest::endpoint;
using namespace psme::rest::registries;

namespace {

json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[constants::Common::ODATA_CONTEXT] =
        "/redfish/v1/$metadata#MessageRegistryFileCollection.MessageRegistryFileCollection";
    r[constants::Common::ODATA_ID] = json::Json::value_t::null;
    r[constants::Common::ODATA_TYPE] = "#MessageRegistryFileCollection.MessageRegistryFileCollection";
    r[constants::Common::NAME] = "MessageRegistryFile collection";
    r[constants::Common::DESCRIPTION] = "Collection of Message Registry Files";
    r[constants::Collection::ODATA_COUNT] = json::Json::value_t::null;
    r[constants::Collection::MEMBERS] = json::Json::value_t::array;

    return r;
}

}


MessageRegistryFileCollection::MessageRegistryFileCollection(const std::string& path) : EndpointBase(path) { }


MessageRegistryFileCollection::~MessageRegistryFileCollection() { }


void MessageRegistryFileCollection::get(const server::Request& request, server::Response& response) {
    auto json = make_prototype();

    json[constants::Common::ODATA_ID] = PathBuilder(request).build();
    json[constants::Collection::ODATA_COUNT] = MessageRegistryFileManager::get_instance()->get_count();

    for (const auto& file : MessageRegistryFileManager::get_instance()->get_files()) {
        json::Json link_elem(json::Json::value_t::object);
        link_elem[constants::Common::ODATA_ID] = PathBuilder(request).append(file.get_id()).build();
        json[constants::Collection::MEMBERS].push_back(std::move(link_elem));
    }

    set_response(response, json);
}
