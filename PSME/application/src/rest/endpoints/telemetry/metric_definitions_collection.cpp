/*!
 * @brief Metric definitions collection endpoint
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file metric_definitions_collection.cpp
 */

#include "psme/rest/endpoints/telemetry/metric_definitions_collection.hpp"



using namespace psme::rest::constants;
using namespace psme::rest::endpoint;

namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Intel_RackScale.MetricDefinitionCollection.MetricDefinitionCollection";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#Intel_RackScale.MetricDefinitionCollection.MetricDefinitionCollection";
    r[Common::NAME] = "Metric Definition Collection";
    r[Common::DESCRIPTION] = "Metric Definition Collection";
    r[Collection::ODATA_COUNT] = 0;
    r[Collection::MEMBERS] = json::Json::value_t::array;

    return r;
}
}

MetricDefinitionsCollection::MetricDefinitionsCollection(const std::string& path) : EndpointBase(path) {}


MetricDefinitionsCollection::~MetricDefinitionsCollection() {}


void MetricDefinitionsCollection::get(const server::Request& request, server::Response& response) {
    auto json = ::make_prototype();

    json[Common::ODATA_ID] = PathBuilder(request).build();

    auto definition_ids = agent_framework::module::CommonComponents::get_instance()
        ->get_metric_definition_manager().get_ids();

    json[Collection::ODATA_COUNT] = std::uint32_t(definition_ids.size());

    for (const auto& id : definition_ids){
        json::Json link = json::Json();
        link[Common::ODATA_ID] = PathBuilder(request).append(id).build();
        json[Collection::MEMBERS].push_back(std::move(link));
    }

    set_response(response, json);
}
