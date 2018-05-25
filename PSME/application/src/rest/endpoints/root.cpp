/*!
 * @copyright
 * Copyright (c) 2015-2018 Intel Corporation
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

#include "agent-framework/service_uuid.hpp"
#include "configuration/configuration.hpp"

#include "psme/rest/constants/constants.hpp"
#include "psme/rest/endpoints/root.hpp"

#include "version.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;

using agent_framework::generic::ServiceUuid;

namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#ServiceRoot.ServiceRoot";
    r[Common::ODATA_ID] = "/redfish/v1";
    r[Common::ODATA_TYPE] = "#ServiceRoot.v1_1_1.ServiceRoot";
    r[Common::ID] = "RootService";
    r[Common::NAME] = "PSME Service Root";
    r[Common::DESCRIPTION] = "Service Root description";
    r[Root::REDFISH_VERSION] = "1.1.0";
    r[Common::UUID] = "00000000000-000000-0000000-00000000000000000";
    r[Root::EVENT_SERVICE][Common::ODATA_ID] = "/redfish/v1/EventService";
    r[Root::UPDATE_SERVICE][Common::ODATA_ID] = "/redfish/v1/UpdateService";
    r[Root::TASKS][Common::ODATA_ID] = "/redfish/v1/TaskService";
    r[Root::REGISTRIES][Common::ODATA_ID] = "/redfish/v1/Registries";
    r[Common::CHASSIS][Common::ODATA_ID] = "/redfish/v1/Chassis";
    r[Root::SYSTEMS][Common::ODATA_ID] = "/redfish/v1/Systems";
    r[Root::MANAGERS][Common::ODATA_ID] = "/redfish/v1/Managers";
    r[Root::FABRICS][Common::ODATA_ID] = "/redfish/v1/Fabrics";
    r[Root::TELEMETRY_SERVICE][Common::ODATA_ID] = "/redfish/v1/TelemetryService";
    r[Root::STORAGE_SERVICES][Common::ODATA_ID] = "/redfish/v1/StorageServices";
    r[Common::OEM][Common::RACKSCALE][Common::ODATA_TYPE] = "#Intel.Oem.ServiceRoot";
    r[Common::OEM][Common::RACKSCALE][Common::API_VERSION] = psme::app::Version::to_string();
    r[Common::OEM][Common::RACKSCALE][Root::ETHERNET_SWITCHES][Common::ODATA_ID] = "/redfish/v1/EthernetSwitches";
    r[Common::LINKS] = json::Value::Type::OBJECT;
    return r;
}
}


endpoint::Root::Root(const std::string& path) : EndpointBase(path) {
    const auto& config = configuration::Configuration::get_instance().to_json();
    service_root_name = config["rest"]["service-root-name"].as_string();
}


endpoint::Root::~Root() { }


void endpoint::Root::get(const server::Request&, server::Response& response) {
    auto json = make_prototype();

    json[Common::UUID] = ServiceUuid::get_instance()->get_service_uuid();
    json[Common::NAME] = service_root_name;

    set_response(response, json);
}
