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
#include "psme/rest/endpoints/system/volumes_collection.hpp"



using namespace psme::rest::endpoint;
using namespace psme::rest::constants;

namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#VolumeCollection.VolumeCollection";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#VolumeCollection.VolumeCollection";
    r[Common::NAME] = "Computer Volume Collection";
    r[Common::DESCRIPTION] = "Computer Volume Collection";
    r[Collection::ODATA_COUNT] = 0;
    r[Collection::MEMBERS] = json::Json::value_t::array;

    return r;
}
}


VolumesCollection::VolumesCollection(const std::string& path) : EndpointBase(path) {}


VolumesCollection::~VolumesCollection() {}


void VolumesCollection::get(const server::Request& req, server::Response& res) {
    auto json = ::make_prototype();

    json[Common::ODATA_ID] = PathBuilder(req).build();

    // run Find to check if System & StorageSubsystem exist
    psme::rest::model::find<agent_framework::model::System, agent_framework::model::StorageSubsystem>(req.params).get();

    set_response(res, json);
}
