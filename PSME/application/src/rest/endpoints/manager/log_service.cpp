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

#include "psme/rest/endpoints/manager/log_service.hpp"
#include "psme/rest/constants/constants.hpp"
#include "psme/rest/utils/status_helpers.hpp"
#include "psme/rest/security/session/session_service_manager.hpp"
#include "agent-framework/module/common_components.hpp"

using namespace psme::rest;
using namespace psme::rest::constants;
using namespace agent_framework::module;

namespace {

json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#LogService.LogService";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#LogService.v1_1_1.LogService";
    r[Common::ID] = json::Json::value_t::null;
    r[Common::NAME] = "Log Service";
    r[Common::DESCRIPTION] = "Log Service description";

    r[LogService::LOG_ENTRY_TYPE] = json::Json::value_t::null;
    r[LogService::OVERWRITE_POLICY] = json::Json::value_t::null;
    r[Common::SERVICE_ENABLED] = true;
    r[LogService::ENTRIES] = json::Json::value_t::null;

    r[Common::STATUS][Common::STATE] = "Enabled";
    r[Common::STATUS][Common::HEALTH] = "OK";
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Json::value_t::null;

    return r;
}
}


endpoint::LogService::LogService(const std::string& path) : EndpointBase(path) {}


endpoint::LogService::~LogService() {}


void endpoint::LogService::get(const server::Request& request, server::Response& response) {
    auto r = ::make_prototype();

    auto log_service = psme::rest::model::find<agent_framework::model::Manager, agent_framework::model::LogService>(request.params).get();

    endpoint::utils::fill_name_and_description(log_service, r);

    r[Common::ODATA_ID] = PathBuilder(request).build();
    r[Common::ID] = log_service.get_id();
    r[constants::LogService::LOG_ENTRY_TYPE] = log_service.get_log_entry_type();
    r[constants::LogService::OVERWRITE_POLICY] = log_service.get_overwrite_policy();
    r[constants::LogService::ENTRIES][Common::ODATA_ID] = PathBuilder(request).append(constants::LogService::ENTRIES).build();

    set_response(response, r);
}
