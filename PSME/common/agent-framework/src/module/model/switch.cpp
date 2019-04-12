/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * */

#include "agent-framework/module/model/switch.hpp"
#include "agent-framework/module/constants/pnc.hpp"
#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::attribute;

const enums::Component Switch::component = enums::Component::Switch;
const enums::CollectionName Switch::collection_name =
    enums::CollectionName::Switches;

Switch::Switch(const std::string& parent_uuid, enums::Component parent_type) :
    Resource{parent_uuid, parent_type} {}

Switch::~Switch() {}

json::Json Switch::to_json() const {
    json::Json result = json::Json();
    result[literals::Switch::SWITCH_ID] = get_switch_id();
    result[literals::Switch::CHASSIS] = get_chassis();
    result[literals::Switch::ASSET_TAG] = get_asset_tag();
    result[literals::Switch::SKU] = get_sku();
    result[literals::Switch::FRU_INFO] = get_fru_info().to_json();
    result[literals::Switch::COLLECTIONS] = get_collections().to_json();
    result[literals::Switch::STATUS] = get_status().to_json();
    result[literals::Switch::OEM] = get_oem().to_json();
    result[literals::Switch::INDICATOR_LED] = get_indicator_led();
    result[literals::Switch::POWER_STATE] = get_power_state();
    result[literals::Switch::ALLOWED_ACTIONS] = get_allowed_actions().to_json();
    result[literals::Switch::PROTOCOL] = get_protocol();
    return result;
}

Switch Switch::from_json(const json::Json& json) {
    Switch fabric_switch{};
    fabric_switch.set_switch_id(json[literals::Switch::SWITCH_ID]);
    fabric_switch.set_chassis(json[literals::Switch::CHASSIS]);
    fabric_switch.set_asset_tag(json[literals::Switch::ASSET_TAG]);
    fabric_switch.set_sku(json[literals::Switch::SKU]);
    fabric_switch.set_fru_info(FruInfo::from_json(json[literals::Switch::FRU_INFO]));
    fabric_switch.set_collections(Collections::from_json(json[literals::Switch::COLLECTIONS]));
    fabric_switch.set_status(Status::from_json(json[literals::Switch::STATUS]));
    fabric_switch.set_oem(Oem::from_json(json[literals::Switch::OEM]));
    fabric_switch.set_power_state(json[literals::Switch::POWER_STATE]);
    fabric_switch.set_allowed_actions(AllowedActions::from_json(json[literals::Switch::ALLOWED_ACTIONS]));
    fabric_switch.set_indicator_led(json[literals::Switch::INDICATOR_LED]);
    fabric_switch.set_protocol(json[literals::Switch::PROTOCOL]);

    return fabric_switch;
}
