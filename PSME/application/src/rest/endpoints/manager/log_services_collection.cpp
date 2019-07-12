/*!
 * @copyright
 * Copyright (c) 2019 Intel Corporation
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

#include "psme/rest/endpoints/endpoints.hpp"
#include "psme/rest/endpoints/manager/log_services_collection.hpp"
#include "psme/rest/constants/constants.hpp"
#include "psme/rest/endpoints/utils.hpp"
#include "agent-framework/module/common_components.hpp"

using namespace psme::rest;
using namespace psme::rest::endpoint;
using namespace psme::rest::constants;
using namespace agent_framework::module;


namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#LogServiceCollection.LogServiceCollection";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#LogServicesCollection.LogServiceCollection";
    r[Common::NAME] = "Log Service Collection";
    r[Common::DESCRIPTION] = "Collection of LogServices for this Manager";
    r[Collection::ODATA_COUNT] = json::Json::value_t::null;
    r[Collection::MEMBERS] = json::Json::value_t::array;

    return r;
}
}

LogServicesCollection::LogServicesCollection(const std::string& path) : EndpointBase(path) {}

LogServicesCollection::~LogServicesCollection() {}

void LogServicesCollection::get(const server::Request& request, server::Response& response) {

    auto r = ::make_prototype();

    r[Common::ODATA_ID] = PathBuilder(request).build();

    auto manager_uuid = psme::rest::model::find<agent_framework::model::Manager>(request.params).get_uuid();
    auto log_service_ids = get_manager<agent_framework::model::LogService>().get_ids(manager_uuid);

    r[Collection::ODATA_COUNT] = static_cast<std::uint32_t>(log_service_ids.size());

    for (const auto& log_service_id : log_service_ids) {
        json::Json link(json::Json::value_t::object);
        link[Common::ODATA_ID] = PathBuilder(request).append(log_service_id).build();
        r[Collection::MEMBERS].push_back(std::move(link));
    }

    set_response(response, r);
}
