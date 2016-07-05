/*!
 * @section LICENSE
 *
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
 *
 * @section DESCRIPTION
 *
 * @file requests/chassis/get_power_zone_info.cpp
 *
 * @brief Generic chassis request get power zone information implementation
 * */

#include \
    "agent-framework/module-ref/requests/compute/set_power_zone_attributes.hpp"
#include "agent-framework/module-ref/constants/compute.hpp"
#include "agent-framework/module-ref/constants/common.hpp"
#include "agent-framework/module-ref/enum/compute.hpp"
#include <json/json.h>


using namespace agent_framework::model::requests;
using namespace agent_framework::model::literals;
using namespace agent_framework::model::enums;


/* WARNING  the OEMData argument is not used. Please change this once OEMData
 * gets a real structure & constructor.*/
SetBladeAttributes::SetBladeAttributes(const std::string& component,
            BootOverride boot_override,
            BootOverrideTarget boot_override_target,
            PowerState power_state,
            agent_framework::model::attribute::OEMData):
                 m_component{component},
                m_boot_override{boot_override},
                m_boot_override_target{boot_override_target},
                m_power_state{power_state},
                m_oem_data{} {}

Json::Value SetBladeAttributes::to_json() const {
    Json::Value value;
    value[Blade::COMPONENT] = m_component;
    value[Blade::BOOT_OVERRIDE] = m_boot_override.to_string();
    value[Blade::BOOT_OVERRIDE_TARGET] = m_boot_override_target.to_string();
    value[Blade::POWER_STATE] = m_power_state.to_string();
    value[literals::OEMData::OEM] = m_oem_data.to_json();
    return value;
}

SetBladeAttributes SetBladeAttributes::from_json(const Json::Value& json) {
    return SetBladeAttributes{
        json[Blade::COMPONENT].asString(),
        BootOverride::from_string(json[Blade::BOOT_OVERRIDE].asString()),
        BootOverrideTarget::from_string(
                json[Blade::BOOT_OVERRIDE_TARGET].asString()),
        PowerState::from_string(json[Blade::POWER_STATE].asString()),
        agent_framework::model::attribute::OEMData{}
    };
}
