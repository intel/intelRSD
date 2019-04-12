/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 *
 * @file discoverer_mf3.cpp
 *
 * */

#include "discovery/discoverer_mf3.hpp"
#include "fru_eeprom.hpp"

using namespace agent_framework::model;
using namespace agent::pnc::discovery;
using namespace agent::pnc::tools;
using namespace agent::pnc::sysfs;

DiscovererMf3::~DiscovererMf3() {}

Switch DiscovererMf3::discover_switch(const std::string& fabric_uuid, const Toolset&,
        const std::string& chassis_uuid, const SysfsSwitch& sysfs_switch) const {
    return m_factory->init_builder(m_factory->get_switch_builder(), fabric_uuid)->update_sysfs(sysfs_switch)
        .update_links(chassis_uuid).build();
}

Chassis DiscovererMf3::discover_chassis(const Toolset& t, const Chassis& chassis) const {
    auto builder = m_factory->init_builder(m_factory->get_chassis_builder(), chassis);
    FruEeprom fru_eeprom{};
    if (t.i2c_tool->get_fru_eeprom(fru_eeprom)) {
        builder->update_fru(fru_eeprom);
    }
    return builder->build();
}
