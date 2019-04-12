/*!
 * @brief Pcie function builder class implementation
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
 * @file pcie_function_builder.cpp
 */

#include "discovery/builders/pcie_function_builder.hpp"
#include "agent-framework/module/utils/to_hex_string.hpp"


using namespace agent::compute::discovery;
using namespace agent_framework::model;
using namespace agent_framework::model::utils;


PcieFunction PcieFunctionBuilder::build_default(const std::string& parent_uuid) {
    PcieFunction pcie_function{parent_uuid};
    pcie_function.set_status({enums::State::Enabled, {}});

    return pcie_function;
}

void PcieFunctionBuilder::update_smbios_data(agent_framework::model::PcieFunction& pcie_function,
                                           const smbios::parser::SmbiosParser::StructEnhanced<smbios::parser::SMBIOS_PCIE_INFO_DATA>& smbios_data) {

    // We have no subclass or program interface. Fill the rest of the class code with zeros.
    pcie_function.set_pci_class_code(to_hex_string<1>(smbios_data.data.ClassCode) + "0000");

    pcie_function.set_pci_vendor_id(to_hex_string<2>(smbios_data.data.VendorID));
    pcie_function.set_pci_device_id(to_hex_string<2>(smbios_data.data.DeviceID));
    pcie_function.set_pci_subsystem_vendor_id(to_hex_string<2>(smbios_data.data.SubVendorID));
    pcie_function.set_pci_subsystem_id(to_hex_string<2>(smbios_data.data.SubDeviceID));
}