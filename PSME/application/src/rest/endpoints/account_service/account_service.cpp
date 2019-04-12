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
 *
 * @file account_service.cpp
 * */

#include "psme/rest/endpoints/account_service/account_service.hpp"
#include "psme/rest/constants/constants.hpp"
#include "psme/rest/utils/status_helpers.hpp"
#include "psme/rest/server/error/error_factory.hpp"
#include "agent-framework/module/enum/common.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;
using namespace agent_framework::model;

namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#AccountService.AccountService";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#AccountService.v1_3_0.AccountService";
    r[Common::ID] = "AccountService";
    r[Common::NAME] = "Account Service";
    r[Common::DESCRIPTION] = "Account Service";
    r[Common::STATUS][Common::STATE] = "Enabled";
    r[Common::STATUS][Common::HEALTH] = "OK";

    r[AccountService::SERVICE_ENABLED] = true;
    r[AccountService::LOCAL_ACCOUNT_AUTH] = "Enabled";

    r[AccountService::ACCOUNTS] = json::Json::value_t::object;
    r[AccountService::ACCOUNTS][Common::ODATA_ID] = "/redfish/v1/AccountService/Accounts";

    r[AccountService::ROLES] = json::Json::value_t::object;
    r[AccountService::ROLES][Common::ODATA_ID] = "/redfish/v1/AccountService/Roles";

    return r;
}
}

namespace psme {
namespace rest {
namespace endpoint {

AccountService::AccountService(const std::string& path) : EndpointBase(path) {}


AccountService::~AccountService() {}


void AccountService::get(const server::Request& req, server::Response& res) {
    auto r = make_prototype();

    r[Common::STATUS][Common::STATE] = enums::State(enums::State::Enabled).to_string();
    r[constants::AccountService::SERVICE_ENABLED] = true;
    r[Common::ODATA_ID] = PathBuilder(req).build();

    set_response(res, r);
}

}
}
}
