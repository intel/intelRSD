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

#include "psme/rest/endpoints/chassis/pcie_functions_collection.hpp"



using namespace psme::rest::endpoint;
using namespace psme::rest::constants;

namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#PCIeFunctionCollection.PCIeFunctionCollection";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#PCIeFunctionCollection.PCIeFunctionCollection";
    r[Common::DESCRIPTION] = "Collection of PCIe Functions";
    r[Common::NAME] = "PCIe Function Collection";
    r[Collection::ODATA_COUNT] = json::Json::value_t::null;
    r[Collection::MEMBERS] = json::Json::value_t::array;

    return r;
}


auto find_pci_devices_ids(const psme::rest::server::Request& req) {

    // the devices are under the same manager as the chassis from the URL
    auto chassis_manager_uuid= psme::rest::model::find<agent_framework::model::Chassis>(
        req.params).get_one()->get_parent_uuid();

    auto chassis_manager_id = agent_framework::module::get_manager<agent_framework::model::Manager>().get_entry(chassis_manager_uuid).get_id();

    auto params_copy = req.params;
    params_copy[PathParam::MANAGER_ID] = std::to_string(chassis_manager_id);

    const auto device_uuid = psme::rest::model::find<agent_framework::model::Manager, agent_framework::model::PcieDevice>(
        params_copy).get_uuid();

    return agent_framework::module::get_manager<agent_framework::model::PcieFunction>().get_ids(device_uuid);
}

}


PcieFunctionsCollection::PcieFunctionsCollection(const std::string& path) : EndpointBase(path) {}


PcieFunctionsCollection::~PcieFunctionsCollection() {}


void PcieFunctionsCollection::get(const server::Request& req, server::Response& res) {
    auto json = ::make_prototype();

    json[Common::ODATA_ID] = PathBuilder(req).build();

    auto function_ids = find_pci_devices_ids(req);

    json[Collection::ODATA_COUNT] = uint32_t(function_ids.size());

    for (const auto& id : function_ids) {
        json::Json link(json::Json::value_t::object);
        link[Common::ODATA_ID] = endpoint::PathBuilder(req)
            .append(id)
            .build();
        json[Collection::MEMBERS].push_back(std::move(link));
    }

    set_response(res, json);
}
