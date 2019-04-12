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

#include "psme/rest/endpoints/redfish.hpp"
#include "psme/rest/endpoints/root.hpp"
#include "psme/rest/constants/constants.hpp"
#include "psme/rest/server/methods_handler.hpp"

using psme::rest::server::Request;
using psme::rest::server::Response;
using namespace psme::rest::endpoint;
using namespace psme::rest;

Redfish::Redfish(const std::string& path) : EndpointBase(path) {}

Redfish::~Redfish() {}

void Redfish::get(const Request&, Response& res) {
    json::Json r(json::Json::value_t::object);
    r[constants::Redfish::V1] = constants::PathParam::BASE_URL;
    set_response(res, r);
}
