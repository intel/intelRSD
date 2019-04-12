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
 * @file pcie_function_builder.cpp
 *
 * @brief PcieFunctionBuilder implementation
 * */

#include "discovery/builders/pcie_function_builder.hpp"
#include "discovery/discovery_utils.hpp"
#include "agent-framework/module/utils/to_hex_string.hpp"

using namespace agent::pnc::discovery::builders;
using namespace agent_framework::model;
using namespace agent_framework::model::utils;
using namespace agent::pnc::sysfs;
using namespace agent::pnc::discovery::utils;

void PcieFunctionBuilder::build_default() {
    attribute::Status status;
    status.set_health(enums::Health::OK);
    status.set_state(enums::State::Enabled);
    m_obj.set_status(std::move(status));
}

PcieFunctionBuilder::ReturnType PcieFunctionBuilder::update_sysfs(const SysfsFunction& sysfs_function) {
    m_obj.set_function_id(std::to_string(static_cast<uint32_t>(sysfs_function.id.function_num)));
    m_obj.set_function_type(sysfs_function.is_virtual ? enums::PcieFunctionType::Virtual :
                                                        enums::PcieFunctionType::Physical);
    m_obj.set_device_class(to_device_class_enum(sysfs_function.device_class));
    m_obj.set_pci_device_id(to_hex_string<2>(sysfs_function.pci_device_id));
    m_obj.set_pci_vendor_id(to_hex_string<2>(sysfs_function.pci_vendor_id));
    m_obj.set_pci_subsystem_id(to_hex_string<2>(sysfs_function.pci_subsystem_id));
    m_obj.set_pci_subsystem_vendor_id(to_hex_string<2>(sysfs_function.pci_subsystem_vendor_id));
    uint32_t full_device_class = (sysfs_function.device_class << 16)
                               + (sysfs_function.device_subclass << 8)
                               + sysfs_function.device_prog_if;
    m_obj.set_pci_class_code(to_hex_string<3>(full_device_class));
    m_obj.set_pci_revision_id(to_hex_string<1>(sysfs_function.pci_revision_id));
    return *this;
}

PcieFunctionBuilder::ReturnType PcieFunctionBuilder::update_links(const std::string& dsp_port_uuid) {
    m_obj.set_dsp_port_uuid(dsp_port_uuid);
    return *this;
}
