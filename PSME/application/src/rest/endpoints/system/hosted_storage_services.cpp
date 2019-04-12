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
 */



#include "psme/rest/constants/constants.hpp"
#include "psme/rest/endpoints/system/hosted_storage_services.hpp"



using namespace psme::rest;
using namespace psme::rest::endpoint;
using namespace psme::rest::constants;

using namespace agent_framework::module;
using namespace agent_framework::model;

namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#HostedStorageServices.HostedStorageServices";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#HostedStorageServices.HostedStorageServices";
    r[Common::NAME] = "Hosted Storage Services Collection";
    r[Common::DESCRIPTION] = "Collection of Storage Services hosted on the Computer System";
    r[Collection::ODATA_COUNT] = json::Json::value_t::null;
    r[Collection::MEMBERS] = json::Json::value_t::array;

    return r;
}

}


HostedStorageServices::HostedStorageServices(const std::string& path) : EndpointBase(path) {}


HostedStorageServices::~HostedStorageServices() {}


void HostedStorageServices::get(const server::Request& request, server::Response& response) {
    auto r = ::make_prototype();

    r[Common::ODATA_ID] = PathBuilder(request).build();

    auto system = psme::rest::model::find<agent_framework::model::System>(request.params).get();
    const auto& hosted_services_ids = agent_framework::module::get_m2m_manager<agent_framework::model::System, agent_framework::model::StorageService>().get_children(
        system.get_uuid());

    r[Collection::ODATA_COUNT] = std::uint32_t(hosted_services_ids.size());

    if (!hosted_services_ids.empty()) {

        for (const auto& id : hosted_services_ids) {
            json::Json link = json::Json();
            link[Common::ODATA_ID] = PathBuilder(constants::Routes::ROOT_PATH).append(
                constants::Common::STORAGE_SERVICES).append(id).build();

            r[Collection::MEMBERS].push_back(std::move(link));
        }
    }
    set_response(response, r);
}