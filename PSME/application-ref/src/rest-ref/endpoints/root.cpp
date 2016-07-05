/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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

#include "service_uuid.hpp"
#include "psme/rest-ref/endpoints/root.hpp"
#include "psme/rest-ref/constants/constants.hpp"
#include "json/json.hpp"
#include "version.hpp"

using namespace psme::rest;
using namespace app::rest;
using namespace app::rest::constants;

namespace {
    json::Value make_prototype() {
        json::Value r(json::Value::Type::OBJECT);

        r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#ServiceRoot.ServiceRoot";
        r[Common::ODATA_ID] = "/redfish/v1";
        r[Common::ODATA_TYPE] = "#ServiceRoot.1.0.0.ServiceRoot";
        r[Common::ID] = "RootService";
        r[Common::NAME] = "Service Root";
        r[Common::DESCRIPTION] = "Service Root description";
        r[Root::REDFISH_VERSION] = "1.0.0";
        r[Common::UUID] = "00000000000-000000-0000000-00000000000000000";
        r[Root::EVENT_SERVICE][Common::ODATA_ID] = "/redfish/v1/EventService";
        r[Root::CHASSIS][Common::ODATA_ID] = "/redfish/v1/Chassis";
        r[Root::SYSTEMS][Common::ODATA_ID] = "/redfish/v1/Systems";
        r[Root::SERVICES][Common::ODATA_ID] = "/redfish/v1/Services";
        r[Root::MANAGERS][Common::ODATA_ID] = "/redfish/v1/Managers";
        r[Root::ETHERNET_SWITCHES][Common::ODATA_ID] = "/redfish/v1/EthernetSwitches";
        r[Common::OEM][Common::RACKSCALE][Common::ODATA_TYPE] = "#Intel.Oem.ServiceRoot";
        r[Common::OEM][Common::RACKSCALE][Common::API_VERSION] = psme::app::Version::to_string();
        r[Common::LINKS] = json::Value::Type::OBJECT;
        return r;
    }
}

endpoint::Root::Root(const std::string& path) : Endpoint(path) {}
endpoint::Root::~Root() {}

void endpoint::Root::get(const server::Request&, server::Response& res) {
    auto json = make_prototype();
    json[Common::UUID] = psme::app::ServiceUuid::get_instance()->get_service_uuid();
    res << json::Serializer(json);
}
