/*!
 * @brief OPAE implementation
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file device.cpp
 */


#include "opaepp/device.hpp"
#include "logger/logger.hpp"
#include "uuid/uuid.hpp"



using namespace opaepp;


std::string Device::get_pci_address(uint8_t bus, uint8_t device, uint8_t function) {
    std::stringstream ss{};
    ss << std::hex << uint32_t(bus) << ':' << uint32_t(device) << '.' << uint32_t(function);
    return ss.str();
}


std::string Device::get_pci_address() const {
    return get_pci_address(m_bus, m_device, m_function);
}


std::ostream& opaepp::operator<<(std::ostream& os, const Device& device) {
    os << "Device Type: " << (device.m_device_type == FPGA_DEVICE ? "FPGA_DEVICE"
                                                                  : device.m_device_type == FPGA_ACCELERATOR
                                                                    ? "FPGA_ACCELERATOR" : "Unknown type");
    os << "\n\tBus                  : 0x" << std::hex << static_cast<uint32_t>(device.m_bus);
    os << "\n\tDevice               : 0x" << std::hex << static_cast<uint32_t>(device.m_device);
    os << "\n\tFunction             : 0x" << std::hex << static_cast<uint32_t>(device.m_function);
    os << "\n\tSocket id            : 0x" << std::hex << static_cast<uint32_t>(device.m_socket_id);
    os << "\n\tDevice id            : 0x" << std::hex << device.m_device_id;
    os << "\n\tGUID                 : " << unparse_uuid(device.m_guid);
    os << "\n\tObject id            : 0x" << std::hex << device.m_object_id;
    if (device.m_device_type == FPGA_DEVICE) {
        os << "\n\tNumber of slots      : " << std::dec << device.m_num_slots;
        os << "\n\tBlue bitstream id    : 0x" << std::hex << device.m_blue_bitstream_id;
        const auto& version = device.m_blue_bitstream_version;
        if (version.has_value()) {
            os << "\n\tMajor version        : " << std::dec << uint32_t(version->major);
            os << "\n\tMinor version        : " << std::dec << uint32_t(version->minor);
            os << "\n\tPatch version        : " << std::dec << version->patch;
        }
        os << "\n\tVendor id            : 0x" << std::hex << device.m_vendor_id;
        os << "\n\tModel                : " << device.m_model;
        os << "\n\tLocal memory size    : " << std::dec << device.m_local_memory_size;
        os << "\n\tCapabilities         : " << std::hex << device.m_capabilities;
    }
    else if (device.m_device_type == FPGA_ACCELERATOR) {
        os << "\n\tMMIO spaces          : " << std::dec << device.m_mmio_spaces;
        os << "\n\tNumber of interrupts : " << std::dec << device.m_num_interrupts;
        os << "\n\tAccelerator state    : " << (device.m_accelerator_state == FPGA_ACCELERATOR_ASSIGNED
                                                ? "FPGA_ACCELERATOR_ASSIGNED" : "FPGA_ACCELERATOR_UNASSIGNED");
    }
    return os << '\n';
}


std::ostream& opaepp::operator<<(std::ostream& os, const std::vector<Device>& devices) {
    auto n = devices.size();
    os << "Found " << n << " devices.\n";
    for (size_t i = 0; i < n; ++i) {
        os << "Device #" << (i + 1) << ":\n" << devices[i];
    }
    return os << std::endl;
}
