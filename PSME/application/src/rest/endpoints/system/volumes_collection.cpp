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
#include "psme/rest/endpoints/system/volumes_collection.hpp"

using namespace psme::rest::endpoint;
using namespace psme::rest::constants;



namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Systems/Members/__SYSTEM_ID__/Storage/Members/__STORAGE_ID__/Volumes/$entity";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#VolumeCollection.VolumeCollection";
    r[Common::NAME] = "Computer Volume Collection";
    r[Common::DESCRIPTION] = "Computer Volume Collection";
    r[Collection::ODATA_COUNT] = 0;
    r[Collection::MEMBERS] = json::Value::Type::ARRAY;

    return r;
}
}

VolumesCollection::VolumesCollection(const std::string& path) : EndpointBase(path) {}
VolumesCollection::~VolumesCollection() {}

void VolumesCollection::get(const server::Request& req, server::Response& res) {
    auto json = ::make_prototype();

    json[Common::ODATA_ID] = PathBuilder(req).build();

    auto system = psme::rest::model::Find<agent_framework::model::System>(req.params[PathParam::SYSTEM_ID]).get();
    auto system_id = system.get_id();
    auto storage =
        psme::rest::model::Find<agent_framework::model::StorageSubsystem>(req.params[PathParam::STORAGE_ID]).
            via<agent_framework::model::System>(req.params[PathParam::SYSTEM_ID]).get();

    json[Common::ODATA_CONTEXT] = std::regex_replace(json[Common::ODATA_CONTEXT].as_string(),
                                                  std::regex("__SYSTEM_ID__"), std::to_string(system_id));
    json[Common::ODATA_CONTEXT] = std::regex_replace(json[Common::ODATA_CONTEXT].as_string(),
                                                  std::regex("__STORAGE_ID__"), std::to_string(storage.get_id()));

    set_response(res, json);
}
