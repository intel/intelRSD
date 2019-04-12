/*!
 * @copyright
 * Copyright (c) 2019 Intel Corporation
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

#include "psme/rest/endpoints/intel_registry.hpp"
#include "psme/rest/registries/config/intel_rackscale_registry.hpp"

using namespace psme::rest::endpoint;

IntelRegistry::IntelRegistry(const std::string &path) : EndpointBase(path) {}

IntelRegistry::~IntelRegistry() {}

void IntelRegistry::get(const Request&, Response& res) {

    json::Json registries = json::Json();
    registries = json::Json::parse(psme::rest::registries::get_intel_rackscale_registry());
    set_response(res, registries);
}