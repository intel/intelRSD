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
 * @file pcie_device_builder.cpp
 *
 * @brief PcieDeviceBuilder implementation
 * */

#include "discovery/builders/pcie_device_builder.hpp"
#include "discovery/discovery_utils.hpp"
#include "agent-framework/module/utils/to_hex_string.hpp"

using namespace agent::pnc::discovery::builders;
using namespace agent_framework::model;
using namespace agent_framework::model::utils;
using namespace agent::pnc::sysfs;
using namespace agent::pnc::discovery::utils;

void PcieDeviceBuilder::build_default() {
    attribute::Status status;
    status.set_health(enums::Health::OK);
    status.set_state(enums::State::Enabled);
    m_obj.set_status(std::move(status));

    m_obj.add_collection(attribute::Collection(
        enums::CollectionName::PcieFunctions,
        enums::CollectionType::PCIeFunctions
    ));
}

PcieDeviceBuilder::ReturnType PcieDeviceBuilder::update_sysfs(const SysfsDevice& sysfs_device) {
    attribute::FruInfo fru_info{};
    fru_info.set_manufacturer(to_hex_string<2>(sysfs_device.vendor_id));
    fru_info.set_model_number(to_hex_string<2>(sysfs_device.device_id));
    if (sysfs_device.has_serial_number) {
        fru_info.set_serial_number(to_hex_string<8, false>(sysfs_device.serial_number));
    }
    m_obj.set_fru_info(std::move(fru_info));

    m_obj.set_device_id(std::to_string(static_cast<uint32_t>(sysfs_device.id.device_num)));
    m_obj.set_device_class(sysfs_device.is_multi_function_device ?
                           enums::PcieDeviceClass::MultiFunction :
                           enums::PcieDeviceClass::SingleFunction);
    return *this;
}

PcieDeviceBuilder::ReturnType PcieDeviceBuilder::update_links(const std::string& chassis_uuid) {
    m_obj.set_chassis(chassis_uuid);
    return *this;
}
