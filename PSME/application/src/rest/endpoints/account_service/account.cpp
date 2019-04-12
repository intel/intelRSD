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
 * @file account.cpp
 * */

#include "psme/rest/endpoints/account_service/account.hpp"
#include "psme/rest/constants/constants.hpp"
#include "psme/rest/server/error/error_factory.hpp"
#include "psme/rest/endpoints/utils.hpp"
#include "psme/rest/security/account/account_manager.hpp"


using namespace psme::rest;
using namespace psme::rest::constants;
using namespace psme::rest::endpoint::utils;
using namespace psme::rest::security::account;

namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#ManagerAccount.ManagerAccount";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#ManagerAccount.v1_1_0.ManagerAccount";
    r[Common::ID] = json::Json::value_t::null;
    r[Common::NAME] = "User Account";
    r[Common::DESCRIPTION] = "User Account";

    r[Account::ENABLED] = json::Json::value_t::null;
    r[Account::PASSWORD] = json::Json::value_t::null;
    r[Account::USER_NAME] = json::Json::value_t::null;
    r[Account::ROLE_ID] = json::Json::value_t::null;
    r[Account::LOCKED] = json::Json::value_t::null;

    r[Common::LINKS][Account::ROLE] = json::Json::value_t::null;

    return r;
}
}

namespace psme {
namespace rest {
namespace endpoint {

Account::Account(const std::string& path) : EndpointBase(path) {}


Account::~Account() {}


void Account::get(const server::Request& req, server::Response& res) {
    auto account_id = id_to_uint64(req.params[PathParam::ACCOUNT_ID]);
    auto account = AccountManager::get_instance()->get(account_id);
    //TODO: account should have role id property

    auto r = make_prototype();
    r[Common::ODATA_ID] = PathBuilder(req).build();
    r[Common::ID] = req.params[PathParam::ACCOUNT_ID];
    r[constants::Account::ENABLED] = account.is_enabled();
    r[constants::Account::USER_NAME] = account.get_user_name();
    r[constants::Account::ROLE_ID] = Common::ADMINISTRATOR;
    r[constants::Account::LOCKED] = account.is_locked();
    r[constants::Account::PASSWORD] = json::Json::value_t::null;

    r[Common::LINKS][constants::Account::ROLE][Common::ODATA_ID] =
        PathBuilder(Routes::ROLES_COLLECTION_PATH).append(Common::ADMINISTRATOR).build();

    set_response(res, r);
}

}
}
}
