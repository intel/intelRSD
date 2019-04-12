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

#include "psme/rest/endpoints/account_service/role.hpp"
#include "psme/rest/security/account/role_manager.hpp"
#include "psme/rest/constants/constants.hpp"
#include "psme/rest/utils/status_helpers.hpp"
#include "psme/rest/server/error/error_factory.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;
using namespace psme::rest::endpoint::utils;
using namespace psme::rest::security::role;


namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Role.Role";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#Role.v1_2_1.Role";
    r[Common::ID] = json::Json::value_t::null;
    r[Common::NAME] = "User Role";
    r[Common::DESCRIPTION] = "Administrator Role";
    r[Role::IS_PREDEFINED] = json::Json::value_t::null;
    r[Role::ASSIGNED_PRIVILEGES] = json::Json::value_t::array;
    r[Role::OEM_PRIVILEGES] = json::Json::value_t::array;
    r[Role::ROLE_ID] = json::Json::value_t::null;

    return r;
}
}


namespace psme {
namespace rest {
namespace endpoint {

Role::Role(const std::string& path) : EndpointBase(path) {}


Role::~Role() { }


void Role::get(const server::Request& req, server::Response& res) {
    auto role_id = req.params[PathParam::ROLE_ID];
    auto role = RoleManager::get_instance()->get(role_id);

    auto r = make_prototype();
    r[constants::Role::ASSIGNED_PRIVILEGES] = role.get_privileges();
    r[Common::ODATA_ID] = PathBuilder(req).build();
    r[Common::ID] = req.params[PathParam::ROLE_ID];
    r[constants::Role::ROLE_ID] = role.get_role_id();
    r[constants::Role::IS_PREDEFINED] = role.is_predefined();
    set_response(res, r);
}

}
}
}
