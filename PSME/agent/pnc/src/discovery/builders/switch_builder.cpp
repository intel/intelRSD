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
 * @file switch_builder.cpp
 *
 * @brief SwitchBuilder implementation
 * */

#include "discovery/builders/switch_builder.hpp"
#include "discovery/discovery_utils.hpp"
#include "agent-framework/module/utils/to_hex_string.hpp"

using namespace agent::pnc::discovery::builders;
using namespace agent_framework::model;
using namespace agent_framework::model::utils;
using namespace agent::pnc::sysfs;
using namespace agent::pnc::discovery::utils;

void SwitchBuilder::build_default() {
    attribute::Status status;
    status.set_health(enums::Health::OK);
    status.set_state(enums::State::Enabled);
    m_obj.set_status(std::move(status));

    m_obj.set_power_state(agent_framework::model::enums::PcieSwitchPowerState::On);
    m_obj.set_protocol(enums::TransportProtocol::PCIe);

    m_obj.add_collection(attribute::Collection(
        enums::CollectionName::Ports,
        enums::CollectionType::Ports
    ));
}

SwitchBuilder::ReturnType SwitchBuilder::update_sysfs(const SysfsSwitch& sysfs_switch) {
    m_obj.set_memory_path(sysfs_switch.memory_resource);
    m_obj.set_bridge_path(sysfs_switch.bridge_path);
    m_obj.set_sec_bus_num(sysfs_switch.sec_bus_num);
    m_obj.set_switch_id(std::to_string(static_cast<uint32_t>(sysfs_switch.bridge_id.device_num)));
    return *this;
}

SwitchBuilder::ReturnType SwitchBuilder::update_seeprom(const Seeprom& seeprom) {
    attribute::FruInfo fru_info{};
    fru_info.set_serial_number(get_null_terminated_string(seeprom.fields.pcie_dev.serial_number,
        Seeprom::SEEPROM_PCIE_DEV_SERIAL_NUMBER_SIZE));
    fru_info.set_manufacturer(to_hex_string<2>(seeprom.fields.pcie_dev.vendor_id));
    fru_info.set_model_number(get_null_terminated_string(seeprom.fields.manufacturer.product_id,
        Seeprom::SEEPROM_MANUFACTURER_PRODUCT_ID_SIZE));
    fru_info.set_part_number(get_null_terminated_string(seeprom.fields.manufacturer.product_rev,
        Seeprom::SEEPROM_MANUFACTURER_PRODUCT_REV_SIZE));
    m_obj.set_fru_info(std::move(fru_info));
    return *this;
}

SwitchBuilder::ReturnType SwitchBuilder::update_links(const std::string& chassis_uuid) {
    m_obj.set_chassis(chassis_uuid);
    return *this;
}
