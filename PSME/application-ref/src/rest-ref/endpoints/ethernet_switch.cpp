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

#include "psme/rest-ref/endpoints/ethernet_switch.hpp"
#include "psme/rest-ref/constants/constants.hpp"
#include "json/json.hpp"

using namespace psme::rest;
using namespace app::rest::constants;



namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] =
        "/redfish/v1/$metadata#EthernetSwitches/Members/$entity";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#EthernetSwitch.1.0.0.EthernetSwitch";
    r[Common::ID] = json::Value::Type::NIL;
    r[Common::NAME] = "Ethernet Switch";
    r[Common::DESCRIPTION] = "Ethernet Switch description";
    r[Switch::SWITCH_ID] = json::Value::Type::NIL;
    r[Common::MANUFACTURER] = json::Value::Type::NIL;
    r[Common::MODEL] = json::Value::Type::NIL;
    r[Switch::MANUFACTURING_DATE] = json::Value::Type::NIL;
    r[Common::SERIAL] = json::Value::Type::NIL;
    r[Common::PART_NUMBER] = json::Value::Type::NIL;
    r[Switch::FIRMWARE_NAME] = json::Value::Type::NIL;
    r[Common::FIRMWARE_VERSION] = json::Value::Type::NIL;
    r[Switch::ROLE] = json::Value::Type::NIL;
    r[Switch::MAX_ACL_NUMBER] = json::Value::Type::NIL;

    r[Common::STATUS][Common::STATE] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Value::Type::NIL;

    r[Common::OEM] = json::Value::Type::OBJECT;

    r[Switch::PORTS] = json::Value::Type::OBJECT;
    r[Switch::ACLS] = json::Value::Type::OBJECT;
    r[Common::LINKS][Switch::CHASSIS] = json::Value::Type::NIL;
    r[Common::LINKS][Common::MANAGED_BY] = json::Value::Type::ARRAY;
    r[Common::LINKS][Common::OEM] = json::Value::Type::OBJECT;

    return r;
}

}

endpoint::EthernetSwitch::EthernetSwitch(const std::string& path) : Endpoint(path) {}
endpoint::EthernetSwitch::~EthernetSwitch() {}

void endpoint::EthernetSwitch::get(const server::Request& req, server::Response& res) {
    auto r = ::make_prototype();

    auto s =  psme::rest::model::Find<agent_framework::model::Switch>
        (req.params[PathParam::ETHERNET_SWITCH_ID]).get();

    r[Common::ODATA_ID] = PathBuilder(req).build();
    r[Common::ID] = req.params[PathParam::ETHERNET_SWITCH_ID];
    r[Common::NAME] = Switch::SWITCH +
        req.params[PathParam::ETHERNET_SWITCH_ID];
    r[Switch::SWITCH_ID] = s.get_switch_identifier();

    const auto& fru = s.get_fru_info();
    r[Common::MANUFACTURER] = fru.get_manufacturer();
    r[Common::MODEL] = fru.get_model_number();
    r[Switch::MANUFACTURING_DATE] = s.get_manufacturing_date();
    r[Common::SERIAL] = fru.get_serial_number();
    r[Common::PART_NUMBER] = fru.get_part_number();
    r[Switch::FIRMWARE_NAME] = s.get_firmware_name();
    r[Common::FIRMWARE_VERSION] = s.get_firmware_version();
    r[Switch::ROLE] = s.get_role();
    psme::rest::endpoint::utils::status_to_json(s,r);
    r[Switch::MAX_ACL_NUMBER] = s.get_max_acl_number();
    r[Common::STATUS][Common::HEALTH_ROLLUP] =
        endpoint::utils::rollup_switch_health(s.get_uuid()).to_string();
    r[Switch::PORTS][Common::ODATA_ID] = PathBuilder(req).append(
        Switch::PORTS).build();
    r[Switch::ACLS][Common::ODATA_ID] = PathBuilder(req).append(
        Switch::ACLS).build();

    r[Common::LINKS]
     [Switch::CHASSIS]
     [Common::ODATA_ID] = PathBuilder(PathParam::BASE_URL)
        .append(Root::CHASSIS)
        .append(utils::get_chassis_by_uuid(s.get_chassis()).get_id()).build();

    json::Value manager;
    manager[Common::ODATA_ID] = PathBuilder(PathParam::BASE_URL)
        .append(Root::MANAGERS)
        .append(utils::get_manager_by_uuid(s.get_parent_uuid()).get_id()).build();

    r[Common::LINKS]
     [Common::MANAGED_BY]
     [Collection::MEMBERS].push_back(std::move(manager));
    set_response(res, r);
}
