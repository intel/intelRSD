/*!
 * @copyright
 * Copyright (c) 2015-2018 Intel Corporation
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

#include "psme/rest/endpoints/chassis/pcie_functions_collection.hpp"



using namespace psme::rest::endpoint;
using namespace psme::rest::constants;

namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#PCIeFunctionCollection.PCIeFunctionCollection";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#PCIeFunctionCollection.PCIeFunctionCollection";
    r[Common::DESCRIPTION] = "Collection of PCIe Functions";
    r[Common::NAME] = "PCIe Function Collection";
    r[Collection::ODATA_COUNT] = json::Value::Type::NIL;
    r[Collection::MEMBERS] = json::Value::Type::ARRAY;

    return r;
}
}


PcieFunctionsCollection::PcieFunctionsCollection(const std::string& path) : EndpointBase(path) {}


PcieFunctionsCollection::~PcieFunctionsCollection() {}


void PcieFunctionsCollection::get(const server::Request& req, server::Response& res) {
    auto json = ::make_prototype();

    json[Common::ODATA_ID] = PathBuilder(req).build();

    // the devices are under the same manager as the chassis from the URL
    auto chassis_manager_uuid = psme::rest::model::Find<agent_framework::model::Chassis>(
        req.params[PathParam::CHASSIS_ID]).get_one()->get_parent_uuid();

    const auto device_uuid =
        psme::rest::model::Find<agent_framework::model::PcieDevice>(req.params[PathParam::DEVICE_ID]).via(
            chassis_manager_uuid).get_uuid();

    auto function_ids =
        agent_framework::module::get_manager<agent_framework::model::PcieFunction>().get_ids(device_uuid);

    json[Collection::ODATA_COUNT] = uint32_t(function_ids.size());

    for (const auto& id : function_ids) {
        json::Value link(json::Value::Type::OBJECT);
        link[Common::ODATA_ID] = endpoint::PathBuilder(req)
            .append(id)
            .build();
        json[Collection::MEMBERS].push_back(std::move(link));
    }

    set_response(res, json);
}
