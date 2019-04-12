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

#include "agent-framework/module/service_uuid.hpp"
#include "configuration/configuration.hpp"

#include "psme/rest/constants/constants.hpp"
#include "psme/rest/endpoints/root.hpp"

#include "version.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;

namespace {

json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#ServiceRoot.ServiceRoot";
    r[Common::ODATA_ID] = "/redfish/v1";
    r[Common::ODATA_TYPE] = "#ServiceRoot.v1_1_1.ServiceRoot";
    r[Common::ID] = "RootService";
    r[Common::NAME] = "PSME Service Root";
    r[Common::DESCRIPTION] = "Service Root description";
    r[Root::REDFISH_VERSION] = "1.1.0";
    r[Common::UUID] = "00000000000-000000-0000000-00000000000000000";
    r[Root::EVENT_SERVICE][Common::ODATA_ID] = "/redfish/v1/EventService";
    r[Root::SESSION_SERVICE][Common::ODATA_ID] = "/redfish/v1/SessionService";
    r[Root::UPDATE_SERVICE][Common::ODATA_ID] = "/redfish/v1/UpdateService";
    r[Root::TASKS][Common::ODATA_ID] = "/redfish/v1/TaskService";
    r[Root::REGISTRIES][Common::ODATA_ID] = "/redfish/v1/Registries";
    r[Common::CHASSIS][Common::ODATA_ID] = "/redfish/v1/Chassis";
    r[Root::SYSTEMS][Common::ODATA_ID] = "/redfish/v1/Systems";
    r[Root::MANAGERS][Common::ODATA_ID] = "/redfish/v1/Managers";
    r[Root::FABRICS][Common::ODATA_ID] = "/redfish/v1/Fabrics";
    r[Root::STORAGE_SERVICES][Common::ODATA_ID] = "/redfish/v1/StorageServices";
    r[Root::ACCOUNT_SERVICE][Common::ODATA_ID] = "/redfish/v1/AccountService";
    r[Common::OEM][Common::RACKSCALE][Common::ODATA_TYPE] = "#Intel.Oem.ServiceRoot";
    r[Common::OEM][Common::RACKSCALE][Common::API_VERSION] = psme::app::Version::to_string();
    r[Common::OEM][Common::RACKSCALE][Root::ETHERNET_SWITCHES][Common::ODATA_ID] = "/redfish/v1/EthernetSwitches";
    r[Common::OEM][Common::RACKSCALE][Root::TELEMETRY_SERVICE][Common::ODATA_ID] = "/redfish/v1/Oem/Intel_RackScale/TelemetryService";
    r[Common::LINKS][SessionService::SESSIONS][Common::ODATA_ID] = "/redfish/v1/SessionService/Sessions";
    return r;
}
}


endpoint::Root::Root(const std::string& path) : EndpointBase(path) {
    const auto& config = configuration::Configuration::get_instance().to_json();
    m_service_root_name = config.value("rest", json::Json())["service-root-name"].get<std::string>();
}


endpoint::Root::~Root() { }


void endpoint::Root::get(const server::Request&, server::Response& response) {
    auto json = make_prototype();

    json[Common::UUID] = agent_framework::module::ServiceUuid::get_instance()->get_service_uuid();
    json[Common::NAME] = m_service_root_name;

    set_response(response, json);
}
