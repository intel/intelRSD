/*!
 * @brief AllocatedPools collection endpoint
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
 * @file allocated_pools_collection.cpp
 */

#include "psme/rest/constants/constants.hpp"
#include "psme/rest/endpoints/storage/allocated_pools_collection.hpp"
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

        r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#StoragePoolCollection.StoragePoolCollection";
        r[Common::ODATA_ID] = json::Json::value_t::null;
        r[Common::ODATA_TYPE] = "#StoragePoolCollection.StoragePoolCollection";
        r[Common::NAME] = "AllocatedPools Collection";
        r[Common::DESCRIPTION] = "Collection of Allocated Pools";
        r[Collection::ODATA_COUNT] = 0;
        r[Collection::MEMBERS] = json::Json::value_t::array;

        return r;
    }

}


AllocatedPoolsCollection::AllocatedPoolsCollection(const std::string& path) : EndpointBase(path) {}


AllocatedPoolsCollection::~AllocatedPoolsCollection() {}


void AllocatedPoolsCollection::get(const server::Request& req, server::Response& res) {
    auto json = ::make_prototype();
    json[constants::Common::ODATA_ID] = PathBuilder(req).build();

    psme::rest::model::find<agent_framework::model::StoragePool>(req.params).get_uuid();

    set_response(res, json);
}

