/*!
 * @brief ProvidedDrives collection endpoint
 *
 * @copyright Copyright (c) 2019 Intel Corporation.
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
 * @file provided_drives_collection.cpp
 */


#include "psme/rest/constants/constants.hpp"
#include "psme/rest/endpoints/storage/providing_drives_collection.hpp"
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

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#DriveCollection.DriveCollection";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#DriveCollection.DriveCollection";
    r[Common::NAME] = "Drives Collection";
    r[Common::DESCRIPTION] = "Collection of Provided Drives";
    r[Collection::ODATA_COUNT] = 0;
    r[Collection::MEMBERS] = json::Json::value_t::array;

    return r;
}


void get_providing_drives(const agent_framework::model::attribute::CapacitySource& capacity_source,
                          json::Json& r) {

    r[Collection::ODATA_COUNT] = static_cast<std::uint32_t>(capacity_source.get_providing_drives().size());
    for (const auto& drive : capacity_source.get_providing_drives()) {
        auto drive_url = psme::rest::endpoint::utils::get_component_url(agent_framework::model::enums::Component::Drive,
                                                                        drive);
        json::Json link = json::Json();
        link[Common::ODATA_ID] = std::move(drive_url);
        r[Collection::MEMBERS].push_back(std::move(link));
    }
}
}


ProvidingDrivesCollection::ProvidingDrivesCollection(const std::string& path) : EndpointBase(path) {}


ProvidingDrivesCollection::~ProvidingDrivesCollection() {}


void ProvidingDrivesCollection::get(const server::Request& request, server::Response& response) {
    auto r = ::make_prototype();
    r[constants::Common::ODATA_ID] = PathBuilder(request).build();

    auto capacity_source_id = utils::id_to_uint64(request.params[PathParam::CAPACITY_SOURCE_ID]);

    auto storage_pool_uuid =
        psme::rest::model::try_find<agent_framework::model::StorageService, agent_framework::model::StoragePool>(
            request.params).get_uuid();

    if (storage_pool_uuid.has_value()) {
        auto storage_pool = get_manager<agent_framework::model::StoragePool>().get_entry(storage_pool_uuid);
        auto capacity_source = storage_pool.get_capacity_sources()[static_cast<std::size_t>(capacity_source_id - 1)];
        get_providing_drives(capacity_source, r);

    }
    else {
        auto volume_uuid =
            psme::rest::model::try_find<agent_framework::model::StorageService, agent_framework::model::Volume>(
                request.params).get_uuid();

        if (!volume_uuid.has_value()) {
            throw agent_framework::exceptions::NotFound("StoragePool or Volume with given ID not found.");
        }

        auto volume = get_manager<agent_framework::model::Volume>().get_entry(volume_uuid);
        auto capacity_source = volume.get_capacity_sources()[static_cast<std::size_t>(capacity_source_id - 1)];
        get_providing_drives(capacity_source, r);
    }
    set_response(response, r);
}

