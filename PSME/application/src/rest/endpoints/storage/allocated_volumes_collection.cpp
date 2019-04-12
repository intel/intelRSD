/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
#include "psme/rest/endpoints/storage/allocated_volumes_collection.hpp"
#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/model/handlers/generic_handler_deps.hpp"



using namespace psme::rest;
using namespace psme::rest::endpoint;
using namespace psme::rest::validators;
using namespace psme::rest::constants;

using namespace agent_framework::module;

namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#VolumeCollection.VolumeCollection";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#VolumeCollection.VolumeCollection";
    r[Common::NAME] = "AllocatedVolumes Collection";
    r[Common::DESCRIPTION] = "Collection of Allocated Volumes";
    r[Collection::ODATA_COUNT] = json::Json::value_t::null;
    r[Collection::MEMBERS] = json::Json::value_t::array;

    return r;
}

}


AllocatedVolumesCollection::AllocatedVolumesCollection(const std::string& path) : EndpointBase(path) {}


AllocatedVolumesCollection::~AllocatedVolumesCollection() {}


void AllocatedVolumesCollection::get(const server::Request& req, server::Response& res) {
    auto json = ::make_prototype();

    json[constants::Common::ODATA_ID] = PathBuilder(req).build();

    auto storage_pool_uuid =
        psme::rest::model::find<agent_framework::model::StorageService, agent_framework::model::StoragePool>(
            req.params).get_uuid();

    const auto& volumes_uuids =
        get_m2m_manager<agent_framework::model::StoragePool, agent_framework::model::Volume>()
            .get_children(storage_pool_uuid);

    json[Collection::ODATA_COUNT] = static_cast<std::uint32_t>(volumes_uuids.size());
    for (const auto& uuid : volumes_uuids) {
        json::Json link(json::Json::value_t::object);
        link[Common::ODATA_ID] = utils::get_component_url(agent_framework::model::enums::Component::Volume, uuid);
        json[Collection::MEMBERS].push_back(std::move(link));
    }

    set_response(res, json);
}

