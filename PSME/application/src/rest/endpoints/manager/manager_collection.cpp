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

#include "psme/rest/endpoints/manager/manager_collection.hpp"
#include "psme/rest/constants/constants.hpp"
#include "psme/rest/endpoints/utils.hpp"

using namespace psme::rest;
using namespace psme::rest::endpoint;
using namespace psme::rest::constants;



namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#ManagerCollection.ManagerCollection";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#ManagerCollection.ManagerCollection";
    r[Common::NAME] = "Manager Collection";
    r[Common::DESCRIPTION] = "Collection of Managers";
    r[Collection::ODATA_COUNT] = json::Json::value_t::null;
    r[Collection::MEMBERS] = json::Json::value_t::array;

    return r;
}
}

ManagerCollection::ManagerCollection(const std::string& path) : EndpointBase(path) {}

ManagerCollection::~ManagerCollection() {}

void ManagerCollection::get(const server::Request& request, server::Response& response) {
    auto json = ::make_prototype();

    json[Common::ODATA_ID] = PathBuilder(request).build();

    auto manager_ids = agent_framework::module::CommonComponents::
        get_instance()->get_module_manager().get_ids();

    json[Collection::ODATA_COUNT] = std::uint32_t(manager_ids.size());

    for (const auto& id : manager_ids) {
        json::Json link = json::Json();
        link[Common::ODATA_ID] = PathBuilder(request).append(id).build();
        json[Collection::MEMBERS].push_back(std::move(link));
    }

    set_response(response, json);
}
