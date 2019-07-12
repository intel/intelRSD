/*!
 * @brief OPAE device reader implementation
 *
 * @copyright Copyright (c) 2019 Intel Corporation
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
 * @file opae_cpp_device_reader.cpp
 */


#include "opaepp/opae/opae_cpp_device_reader.hpp"
#include "logger/logger.hpp"
#include <iostream>
#include <stdexcept>
#include <uuid/uuid.hpp>



using namespace opaepp;
using namespace opae::fpga::types;

namespace {

template<typename Property, typename Type = Property>
OptionalField<Property> read_optional(opae::fpga::types::pvalue<Type>& src,
                                      const std::string& name) {

    Property value{};

    try {

        value = src;
    }
    catch (const opae::fpga::types::not_supported&) {

        log_warning("opaepp", name << " property is not supported");
    }

    return value;
}

}


bool operator!=(const fpga_version& a, const fpga_version& b) {
    return a.major != b.major || a.minor != b.minor || a.patch != b.patch;
}


OpaeCppDeviceReader::OpaeCppDeviceReader() {}


OpaeCppDeviceReader::Devices OpaeCppDeviceReader::get_devices() {

    return read_devices(properties::none);
}


OpaeCppDeviceReader::Devices OpaeCppDeviceReader::get_devices(const PcieDeviceAddress& pcie_device_address) {

    // properties section:

    auto property = properties::get();

    property->bus = pcie_device_address.m_bus_num;
    property->device = pcie_device_address.m_device_num;
    property->function = pcie_device_address.m_func_num;

    // enumeration section:

    std::vector<properties::ptr_t> properties{property};

    return read_devices(properties);
}


OpaeCppDeviceReader::Devices OpaeCppDeviceReader::get_devices(const PcieDeviceAddress& pcie_device_address,
                                                              fpga_objtype obj_type) {

    // properties section:

    auto property = properties::get();

    property->bus = pcie_device_address.m_bus_num;
    property->device = pcie_device_address.m_device_num;
    property->function = pcie_device_address.m_func_num;
    property->type = obj_type;

    // enumeration section::

    std::vector<properties::ptr_t> properties{property};

    return read_devices(properties);
}


Device OpaeCppDeviceReader::discover(const token::ptr_t& token_ptr) {
    Device device;
    read_properties(device, token_ptr);
    return device;
}


OpaeCppDeviceReader::Devices OpaeCppDeviceReader::read_devices(const Properties& properties) {

    auto tokens = token::enumerate(properties);

    Devices devices{};

    for (auto& token : tokens) {

        try {

            auto device = discover(token);

            devices.emplace_back(std::move(device));
        }
        catch (const std::exception& ex) {

            log_error(LOGUSR, "Could not read OPAE device: " << ex.what());
        }
    }

    return devices;
}


void OpaeCppDeviceReader::read_properties(Device& device, const token::ptr_t& token_ptr) {

    auto properties = properties::get(token_ptr);

    device.set_device_type(properties->type);
    device.set_bus(properties->bus);
    device.set_device(properties->device);
    device.set_function(properties->function);
    device.set_socket_id(properties->socket_id);
    device.set_device_type(properties->type);
    device.set_object_id(properties->object_id);
    device.set_device_id(read_optional<uint16_t>(properties->device_id, "device_id"));
    device.set_guid(properties->guid);

    if (device.get_device_type() == FPGA_DEVICE) {

        device.set_num_slots(read_optional<uint32_t>(properties->num_slots, "number of slots"));
        device.set_blue_bitstream_id(read_optional<uint64_t>(properties->bbs_id, "blue bitstream id"));
        device.set_blue_bitstream_version(
            read_optional<fpga_version>(properties->bbs_version, "blue bitstream version"));
        device.set_vendor_id(read_optional<uint16_t>(properties->vendor_id, "vendor id"));
        device.set_model(read_optional<std::string, char*>(properties->model, "model"));
        device.set_local_memory_size(read_optional<uint64_t>(properties->local_memory_size, "local memory size"));
        device.set_capabilities(read_optional<uint64_t>(properties->capabilities, "capabilities"));
    }
    else if (device.get_device_type() == FPGA_ACCELERATOR) {

        device.set_mmio_spaces(read_optional<uint32_t>(properties->num_mmio, "mmio spaces"));
        device.set_num_interrupts(read_optional<uint32_t>(properties->num_interrupts, "number of interrupts"));
        device.set_accelerator_state(
            read_optional<fpga_accelerator_state>(properties->accelerator_state, "accelerator state"));
    }
    else {

        throw std::runtime_error("Unrecognized device type");
    }
}