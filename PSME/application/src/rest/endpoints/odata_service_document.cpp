/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
#include "psme/rest/endpoints/odata_service_document.hpp"

#include <utility>


using namespace psme::rest;
using namespace psme::rest::constants;


namespace {

static std::vector<std::pair<std::string, std::string>> RESOURCES{
    {Root::SERVICE, Routes::ROOT_PATH},
    {Root::SYSTEMS, Routes::SYSTEMS_COLLECTION_PATH},
    {Common::CHASSIS, Routes::CHASSIS_COLLECTION_PATH},
    {Root::MANAGERS, Routes::MANAGER_COLLECTION_PATH},
    {Root::SERVICES, Routes::STORAGE_SERVICE_COLLECTION_PATH},
    {Root::ETHERNET_SWITCHES, Routes::ETHERNET_SWITCHES_COLLECTION_PATH},
    {Root::FABRICS, Routes::FABRICS_COLLECTION_PATH},
    {Root::EVENT_SERVICE, Routes::EVENT_SERVICE_PATH},
    {Root::TASKS, Routes::TASK_SERVICE_PATH},
    {Root::REGISTRIES, Routes::MESSAGE_REGISTRY_FILE_COLLECTION_PATH},
    {Root::UPDATE_SERVICE, Routes::UPDATE_SERVICE_PATH}
};


json::Value make_prototype() {
    json::Value r{};
    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata";

    json::Value values = json::Value::Type::ARRAY;
    for (auto resource : ::RESOURCES) {
        json::Value value;
        value[OdataServiceDocument::NAME] = resource.first;
        value[OdataServiceDocument::KIND] = "Singleton";
        value[OdataServiceDocument::URL] = resource.second;
        values.push_back(std::move(value));
    }
    r[OdataServiceDocument::VALUE] = std::move(values);
    return r;
}
}


endpoint::OdataServiceDocument::OdataServiceDocument(const std::string& path) : EndpointBase(path) { }


endpoint::OdataServiceDocument::~OdataServiceDocument() { }


void endpoint::OdataServiceDocument::get(const server::Request&, server::Response& response) {
    auto json = make_prototype();

    set_response(response, json);
}
