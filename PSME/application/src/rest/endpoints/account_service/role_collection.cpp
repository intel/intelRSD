/*!
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
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

#include <psme/rest/endpoints/endpoint_base.hpp>
#include "psme/rest/endpoints/account_service/role_collection.hpp"
#include "psme/rest/constants/constants.hpp"
#include "psme/rest/utils/status_helpers.hpp"
#include "psme/rest/server/error/error_factory.hpp"
#include "psme/rest/security/account/role_manager.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;
using namespace psme::rest::security::role;

namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#RoleCollection.RoleCollection";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#RoleCollection.RoleCollection";
    r[Common::NAME] = "Roles Collection";
    r[Common::DESCRIPTION] = "Collection of Roles";
    r[Collection::ODATA_COUNT] = json::Json::value_t::null;
    r[Collection::MEMBERS] = json::Json::value_t::array;

    return r;
}
}


namespace psme {
namespace rest {
namespace endpoint {

RoleCollection::RoleCollection(const std::string& path) : EndpointBase(path) {}


RoleCollection::~RoleCollection() { }


void RoleCollection::get(const server::Request& req, server::Response& res) {
    auto r = make_prototype();
    r[Common::ODATA_ID] = PathBuilder(req).build();
    RoleManager::get_instance()->for_each([&r, &req](const auto& role) {
        json::Json link_elem(json::Json::value_t::object);
        link_elem[Common::ODATA_ID] = PathBuilder(req).append(role.get_id()).build();
        r[Collection::MEMBERS].push_back(std::move(link_elem));
    });
    r[Collection::ODATA_COUNT] = r[Collection::MEMBERS].size();
    set_response(res, r);
}

}
}
}