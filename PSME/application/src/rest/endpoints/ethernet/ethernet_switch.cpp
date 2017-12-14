/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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

#include "psme/rest/utils/status_helpers.hpp"
#include "psme/rest/endpoints/ethernet/ethernet_switch.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;

namespace {

json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#EthernetSwitch.EthernetSwitch";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#EthernetSwitch.v1_1_0.EthernetSwitch";
    r[Common::ID] = json::Value::Type::NIL;
    r[Common::NAME] = "Ethernet Switch";
    r[Common::DESCRIPTION] = "Ethernet Switch description";
    r[EthernetSwitch::SWITCH_ID] = json::Value::Type::NIL;
    r[Common::MANUFACTURER] = json::Value::Type::NIL;
    r[Common::MODEL] = json::Value::Type::NIL;
    r[EthernetSwitch::MANUFACTURING_DATE] = json::Value::Type::NIL;
    r[Common::SERIAL_NUMBER] = json::Value::Type::NIL;
    r[Common::PART_NUMBER] = json::Value::Type::NIL;
    r[EthernetSwitch::FIRMWARE_NAME] = json::Value::Type::NIL;
    r[Common::FIRMWARE_VERSION] = json::Value::Type::NIL;
    r[EthernetSwitch::ROLE] = json::Value::Type::NIL;
    r[EthernetSwitch::MAX_ACL_NUMBER] = json::Value::Type::NIL;

    r[Common::STATUS][Common::STATE] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Value::Type::NIL;

    r[Common::OEM] = json::Value::Type::OBJECT;

    r[EthernetSwitch::PORTS] = json::Value::Type::OBJECT;
    r[EthernetSwitch::ACLS] = json::Value::Type::OBJECT;
    r[Common::LINKS][Common::CHASSIS] = json::Value::Type::NIL;
    r[Common::LINKS][Common::MANAGED_BY] = json::Value::Type::ARRAY;
    r[Common::LINKS][Common::OEM] = json::Value::Type::OBJECT;

    return r;
}

}


endpoint::EthernetSwitch::EthernetSwitch(const std::string& path) : EndpointBase(path) {}


endpoint::EthernetSwitch::~EthernetSwitch() {}


void endpoint::EthernetSwitch::get(const server::Request& request, server::Response& response) {
    auto r = ::make_prototype();
    auto s = model::Find<agent_framework::model::EthernetSwitch>(request.params[PathParam::ETHERNET_SWITCH_ID]).get();

    r[Common::ODATA_ID] = PathBuilder(request).build();
    r[Common::ID] = request.params[PathParam::ETHERNET_SWITCH_ID];
    r[Common::NAME] = constants::EthernetSwitch::SWITCH + request.params[PathParam::ETHERNET_SWITCH_ID];
    r[constants::EthernetSwitch::SWITCH_ID] = s.get_switch_identifier();

    const auto& fru = s.get_fru_info();
    r[Common::MANUFACTURER] = fru.get_manufacturer();
    r[Common::MODEL] = fru.get_model_number();
    r[constants::EthernetSwitch::MANUFACTURING_DATE] = s.get_manufacturing_date();
    r[Common::SERIAL_NUMBER] = fru.get_serial_number();
    r[Common::PART_NUMBER] = fru.get_part_number();
    r[constants::EthernetSwitch::FIRMWARE_NAME] = s.get_firmware_name();
    r[Common::FIRMWARE_VERSION] = s.get_firmware_version();
    r[constants::EthernetSwitch::ROLE] = s.get_role();
    r[constants::EthernetSwitch::MAX_ACL_NUMBER] = s.get_max_acl_number();

    psme::rest::endpoint::status_to_json(s, r);
    r[Common::STATUS][Common::HEALTH_ROLLUP] = endpoint::HealthRollup<agent_framework::model::EthernetSwitch>()
        .get(s.get_uuid());
    r[constants::EthernetSwitch::PORTS][Common::ODATA_ID] = PathBuilder(request)
        .append(constants::EthernetSwitch::PORTS)
        .build();
    r[constants::EthernetSwitch::ACLS][Common::ODATA_ID] = PathBuilder(request)
        .append(constants::EthernetSwitch::ACLS)
        .build();

    r[Common::LINKS][constants::Common::CHASSIS][Common::ODATA_ID] =
        utils::get_component_url(agent_framework::model::enums::Component::Chassis, s.get_chassis());

    json::Value manager;
    manager[Common::ODATA_ID] =
        utils::get_component_url(agent_framework::model::enums::Component::Manager, s.get_parent_uuid());

    r[Common::LINKS][Common::MANAGED_BY].push_back(std::move(manager));

    set_response(response, r);
}
