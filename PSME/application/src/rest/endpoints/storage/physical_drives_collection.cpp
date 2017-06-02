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

#include "psme/rest/endpoints/storage/physical_drives_collection.hpp"
#include "psme/rest/constants/constants.hpp"

using namespace psme::rest::endpoint;
using namespace psme::rest::constants;
using namespace agent_framework::module;



namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Drives";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#PhysicalDriveCollection.PhysicalDriveCollection";
    r[Common::NAME] = "Physical Drives Collection";
    r[Common::DESCRIPTION] = "Collection of Physical Drives";
    r[Collection::ODATA_COUNT] = json::Value::Type::NIL;
    r[Collection::MEMBERS] = json::Value::Type::ARRAY;

    return r;
}
}

PhysicalDrivesCollection::PhysicalDrivesCollection(const std::string& path) : EndpointBase(path) {}
PhysicalDrivesCollection::~PhysicalDrivesCollection() {}

void PhysicalDrivesCollection::get(const server::Request& req, server::Response& res) {
    auto json = ::make_prototype();

    json[Common::ODATA_ID] = PathBuilder(req).build();

    auto service_uuid = psme::rest::model::Find<agent_framework::model::StorageServices>(req.params[PathParam::SERVICE_ID]).get_uuid();

    auto keys = StorageComponents::get_instance()->
                        get_physical_drive_manager().get_ids(service_uuid);

    json[Collection::ODATA_COUNT] =
                                    static_cast<std::uint32_t>(keys.size());
    for (const auto& key : keys) {
        json::Value link_elem(json::Value::Type::OBJECT);
        link_elem[Common::ODATA_ID] = PathBuilder(req).append(key).build();
        json[Collection::MEMBERS].push_back(std::move(link_elem));
    }

    set_response(res, json);
}
