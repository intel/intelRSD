/*!
 * @brief Capacity source implementation.
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
 * @file capacity_source.cpp
 * */

#include "json-wrapper/json-wrapper.hpp"
#include "psme/rest/constants/constants.hpp"
#include "psme/rest/endpoints/storage/capacity_source.hpp"

#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/utils/status_helpers.hpp"
#include "psme/rest/server/error/error_factory.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;
using namespace psme::rest::validators;

using namespace psme::rest::constants;
namespace {
json::Json make_prototype() {

    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Capacity.CapacitySource";

    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#Capacity.v1_1_0.CapacitySource";
    r[Common::ID] = json::Json::value_t::null;
    r[Common::NAME] = "CapacitySource";
    r[Common::DESCRIPTION] = "Capacity Source description";

    r[Swordfish::PROVIDED_CAPACITY][Data::DATA][Data::ALLOCATED_BYTES] = json::Json::value_t::null;
    r[Swordfish::PROVIDED_CAPACITY][Data::DATA][Data::CONSUMED_BYTES] = json::Json::value_t::null;
    r[Swordfish::PROVIDED_CAPACITY][Data::DATA][Data::GUARANTEED_BYTES] = json::Json::value_t::null;
    r[Swordfish::PROVIDED_CAPACITY][Data::DATA][Data::PROVISIONED_BYTES] = json::Json::value_t::null;

    r[Swordfish::PROVIDING_POOLS] = json::Json::value_t::null;
    r[Swordfish::PROVIDING_DRIVES] = json::Json::value_t::null;
    r[Swordfish::PROVIDING_VOLUMES] = json::Json::value_t::null;

    r[Common::OEM] = json::Json::value_t::object;

    return r;
}


void set_provided_capacity(const agent_framework::model::attribute::CapacitySource& capacity_source,
                           json::Json& r) {

    r[Swordfish::PROVIDED_CAPACITY][Data::DATA][Data::ALLOCATED_BYTES] = capacity_source.get_allocated_bytes();
    r[Swordfish::PROVIDED_CAPACITY][Data::DATA][Data::CONSUMED_BYTES] = capacity_source.get_consumed_bytes();
    r[Swordfish::PROVIDED_CAPACITY][Data::DATA][Data::GUARANTEED_BYTES] = capacity_source.get_guaranteed_bytes();
    r[Swordfish::PROVIDED_CAPACITY][Data::DATA][Data::PROVISIONED_BYTES] = capacity_source.get_provisioned_bytes();
}


void validate_request(std::uint64_t request_id, std::size_t collection_size) {

    if (request_id > collection_size) {
        std::stringstream ss{};
        ss << "Cannot find capacity source with id: " << request_id;
        THROW(agent_framework::exceptions::NotFound, "rest", ss.str());
    }
}
}


endpoint::CapacitySource::CapacitySource(const std::string& path) : EndpointBase(path) {}


endpoint::CapacitySource::~CapacitySource() {}


void endpoint::CapacitySource::get(const server::Request& request, server::Response& response) {

    auto r = ::make_prototype();

    auto request_id = utils::id_to_uint64(request.params[PathParam::CAPACITY_SOURCE_ID]);

    auto volume =
        model::try_find<agent_framework::model::StorageService, agent_framework::model::Volume>(request.params).get();

    if (volume.has_value()) {
        validate_request(request_id, volume.value().get_capacity_sources().size());
        set_provided_capacity(volume.value().get_capacity_sources()[static_cast<std::size_t>(request_id - 1)], r);
    }
    else {
        auto storage_pool =
            model::try_find<agent_framework::model::StorageService, agent_framework::model::StoragePool>(
                request.params).get();

        if (!storage_pool.has_value()) {
            throw agent_framework::exceptions::NotFound("StoragePool or Volume with given ID not found.");
        }
        validate_request(request_id, storage_pool.value().get_capacity_sources().size());
        set_provided_capacity(storage_pool.value().get_capacity_sources()[static_cast<std::size_t>(request_id - 1)], r);
    }

    r[constants::Common::ID] = request.params[PathParam::CAPACITY_SOURCE_ID];
    r[constants::Common::ODATA_ID] = PathBuilder(request).build();

    json::Json link = json::Json();
    link[Common::ODATA_ID] = endpoint::PathBuilder(request).append(constants::Swordfish::PROVIDING_POOLS).build();
    r[Swordfish::PROVIDING_POOLS].push_back(link);

    link[Common::ODATA_ID] = endpoint::PathBuilder(request).append(constants::Swordfish::PROVIDING_DRIVES).build();
    r[Swordfish::PROVIDING_DRIVES].push_back(link);

    link[Common::ODATA_ID] = endpoint::PathBuilder(request).append(constants::Swordfish::PROVIDING_VOLUMES).build();
    r[Swordfish::PROVIDING_VOLUMES].push_back(link);

    set_response(response, r);
}




