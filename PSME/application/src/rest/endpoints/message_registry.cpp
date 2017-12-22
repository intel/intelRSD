/*!
 * @copyright
 * Copyright (c) 2017 Intel Corporation
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

#include "psme/rest/server/error/server_exception.hpp"
#include "psme/rest/server/error/error_factory.hpp"
#include "psme/rest/constants/constants.hpp"
#include "psme/rest/endpoints/message_registry.hpp"
#include "psme/rest/registries/managers/message_registry_manager.hpp"



using namespace psme::rest;
using namespace psme::rest::endpoint;
using namespace psme::rest::endpoint::utils;

namespace {

json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[constants::Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#MessageRegistry.MessageRegistry";
    r[constants::Common::ODATA_ID] = json::Value::Type::NIL;
    r[constants::Common::ODATA_TYPE] = "#MessageRegistry.v1_0_0.MessageRegistry";
    r[constants::MessageRegistry::LANGUAGE] = json::Value::Type::NIL;
    r[constants::MessageRegistry::MESSAGES] = json::Value::Type::NIL;
    r[constants::MessageRegistry::OWNING_ENTITY] = json::Value::Type::NIL;
    r[constants::MessageRegistry::REGISTRY_PREFIX] = json::Value::Type::NIL;
    r[constants::MessageRegistry::REGISTRY_VERSION] = json::Value::Type::NIL;

    return r;
}

}


MessageRegistry::MessageRegistry(const std::string& path) : EndpointBase(path) {}


MessageRegistry::~MessageRegistry() {}


void MessageRegistry::get(const server::Request& request, server::Response& response) {
    auto r = make_prototype();

    // @TODO: Use MessageRegistryManager to obtain registry
    r[constants::Common::ODATA_ID] = request.get_url();

    set_response(response, r);
}
