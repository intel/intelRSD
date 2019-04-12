/*!
 * @brief Pcie device builder class implementation
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file pcie_device_builder.cpp
 */

#include "discovery/builders/pcie_device_builder.hpp"
#include "agent-framework/module/utils/to_hex_string.hpp"


using namespace agent::compute::discovery;
using namespace agent_framework::model;
using namespace agent_framework::model::utils;


PcieDevice PcieDeviceBuilder::build_default(const std::string& parent_uuid) {
    PcieDevice pcie_device{parent_uuid};
    pcie_device.set_status({enums::State::Enabled, {}});

    pcie_device.add_collection({enums::CollectionName::PcieFunctions,
                                enums::CollectionType::PCIeFunctions
                               });

    return pcie_device;
}


void PcieDeviceBuilder::update_chassis_link(PcieDevice& pcie_device, const std::string& chassis_uuid) {
    pcie_device.set_chassis(chassis_uuid);
}


void PcieDeviceBuilder::update_smbios_data(PcieDevice& pcie_device,
    const smbios::parser::SmbiosParser::StructEnhanced<smbios::parser::SMBIOS_PCIE_INFO_DATA>& smbios_data) {

    std::string vendor_id = to_hex_string<2>(smbios_data.data.VendorID);
    std::string device_id = to_hex_string<2>(smbios_data.data.DeviceID);

    attribute::FruInfo fru_info{};
    fru_info.set_manufacturer(vendor_id);
    fru_info.set_model_number(device_id);
    pcie_device.set_fru_info(std::move(fru_info));

    pcie_device.set_device_id(device_id);
}
