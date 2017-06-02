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
 *
 * @file pci_device.cpp
 * @brief SMBIOS-way of discovering pci devices on a system
 * */

#include "agent-framework/module/compute_components.hpp"
#include "discovery/smbios/pci_device.hpp"

using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace smbios::parser;

namespace agent {
namespace compute {
namespace discovery {
namespace smbios {

std::tuple<bool, enums::Health> discover_pci_devices(SmbiosParser::Ptr smbios, System& system) {
    if (!smbios) {
        return std::make_tuple(false, enums::Health::OK);
    }

    auto smbios_pci_devices = smbios->get_all<SMBIOS_PCIE_INFO_DATA>();
    for (const auto& pci_device_info : smbios_pci_devices) {

        std::stringstream stream;
        stream << "0x" << std::hex << pci_device_info.data.VendorID;
        std::string vendor_id = stream.str();
        stream.str("");
        stream << "0x" << std::hex << pci_device_info.data.DeviceID;
        std::string device_id = stream.str();

        attribute::PciDevice pci_device(vendor_id, device_id);
        system.add_pci_device(pci_device);
    }

    log_info(GET_LOGGER("smbios-discovery"), "PCI Devices Discovery was successful.");
    return std::make_tuple(true, enums::Health::OK);
}

}
}
}
}
