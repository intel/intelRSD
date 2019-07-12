/*!
 * @brief OPAE proxy device reader implementation
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
 * @file opae_proxy_device_reader.cpp
 */


#include <memory>
#include "opaepp/opae-proxy/opae_proxy_device_reader.hpp"
#include "logger/logger.hpp"
#include "opae-proxy/opae_proxy.h"
#include "uuid/uuid.hpp"
#include "opaepp_error.hpp"



using namespace opaepp;

namespace {

using PropertyContext = std::pair<struct fpgaof*, fpga_properties*>;


template<typename T>
void read(T& property, std::function<fpga_result(struct fpgaof*, fpga_properties, T*)> function,
          PropertyContext& context, const std::string& name) {
    if (function(context.first, *context.second, &property) != FPGA_OK) {
        throw OpaeppError("Could not read device " + name);
    }
}


template<typename T>
void read_optional(OptionalField<T>& property,
                   std::function<fpga_result(struct fpgaof*, fpga_properties, T*)> function,
                   PropertyContext& context, const std::string& name) {
    T tmp_value;
    fpga_result ret = function(context.first, *context.second, &tmp_value);
    if (ret == FPGA_NOT_SUPPORTED) {
        log_warning("opaepp", "reading device " << name << " is not supported.");
    }
    else if (ret == FPGA_OK) {
        property = tmp_value;
    }
    else {
        throw OpaeppError("Could not read device " + name);
    }
}


void read_optional_string(OptionalField<std::string>& property,
                          std::function<fpga_result(struct fpgaof*, fpga_properties, char*)> function,
                          PropertyContext& context, const std::string& name) {
    char tmp_value[255]; // should be FPGA_MODEL_LENGTH - not implemented in OPAE
    fpga_result ret = function(context.first, *context.second, tmp_value);
    if (ret == FPGA_NOT_SUPPORTED) {
        log_warning("opaepp", "reading device " << name << " is not supported.");
        return;
    }
    else if (ret == FPGA_OK) {
        property = std::string(tmp_value);
    }
    else {
        throw OpaeppError("Could not read device " + name);
    }
}
}


OpaeProxyDeviceReader::Devices OpaeProxyDeviceReader::get_devices() {

    // properties section:

    fpga_properties properties = nullptr;

    // enumeration section:

    return read_devices(properties, true);
}


OpaeProxyDeviceReader::Devices OpaeProxyDeviceReader::get_devices(const PcieDeviceAddress& pcie_device_address) {

    // properties section:

    fpga_properties properties{};
    auto backend_ptr = m_proxy_context->get_backend();

    auto result = fpgaofGetProperties(backend_ptr, nullptr, &properties);
    OpaeApiError::throw_if_unexpected_result(result, "fpgaofGetProperties");

    result = fpgaofPropertiesSetBus(backend_ptr, properties, pcie_device_address.m_bus_num);
    OpaeApiError::throw_if_unexpected_result(result, "fpgaofPropertiesSetBus");
    result = fpgaofPropertiesSetDevice(backend_ptr, properties, pcie_device_address.m_device_num);
    OpaeApiError::throw_if_unexpected_result(result, "fpgaofPropertiesSetDevice");
    result = fpgaofPropertiesSetFunction(backend_ptr, properties, pcie_device_address.m_func_num);
    OpaeApiError::throw_if_unexpected_result(result, "fpgaofPropertiesSetFunction");

    // enumeration section::

    return read_devices(properties);
}


OpaeProxyDeviceReader::Devices OpaeProxyDeviceReader::get_devices(const PcieDeviceAddress& pcie_device_address,
                                                                  fpga_objtype obj_type) {

    // properties section:

    fpga_properties properties{};
    auto backend_ptr = m_proxy_context->get_backend();

    auto result = fpgaofGetProperties(backend_ptr, nullptr, &properties);
    OpaeApiError::throw_if_unexpected_result(result, "fpgaofGetProperties");

    result = fpgaofPropertiesSetBus(backend_ptr, properties, pcie_device_address.m_bus_num);
    OpaeApiError::throw_if_unexpected_result(result, "fpgaofPropertiesSetBus");
    result = fpgaofPropertiesSetDevice(backend_ptr, properties, pcie_device_address.m_device_num);
    OpaeApiError::throw_if_unexpected_result(result, "fpgaofPropertiesSetDevice");
    result = fpgaofPropertiesSetFunction(backend_ptr, properties, pcie_device_address.m_func_num);
    OpaeApiError::throw_if_unexpected_result(result, "fpgaofPropertiesSetFunction");
    result = fpgaofPropertiesSetObjectType(backend_ptr, properties, obj_type);
    OpaeApiError::throw_if_unexpected_result(result, "fpgaofPropertiesSetObjectType");

    // enumeration section::

    return read_devices(properties);
}


Device OpaeProxyDeviceReader::discover(fpga_token& token) {
    Device device;
    read_properties(device, token);
    return device;
}


OpaeProxyDeviceReader::Devices OpaeProxyDeviceReader::read_devices(fpga_properties& properties,
                                                                   bool key_to_token_mapping) {

    fpga_result result{};
    uint32_t devices_count{};
    std::vector<fpga_token> tokens{};

    if (properties) {

        result = fpgaofEnumerate(m_proxy_context->get_backend(), &properties, 1, NULL, 0, &devices_count);

        OpaeApiError::throw_if_unexpected_result(result, "fpgaofEnumerate", "Could not get number of devices");

        tokens.resize(devices_count);

        result = fpgaofEnumerate(m_proxy_context->get_backend(), &properties, 1, tokens.data(),
                                 devices_count, &devices_count);
    }
    else {

        result = fpgaofEnumerate(m_proxy_context->get_backend(), NULL, 0, NULL, 0, &devices_count);

        OpaeApiError::throw_if_unexpected_result(result, "fpgaofEnumerate", "Could not get number of devices");

        tokens.resize(devices_count);

        result = fpgaofEnumerate(m_proxy_context->get_backend(), NULL, 0, tokens.data(),
                                 devices_count, &devices_count);
    }

    OpaeApiError::throw_if_unexpected_result(result, "fpgaofEnumerate", "Could not get tokens for device discovery");

    Devices devices{};
    devices.reserve(tokens.size());

    FpgaUniqueKeyToTokenMap fpga_unique_key_to_token_map{};

    for (auto& token : tokens) {

        try {
            Device device = discover(token);

            if (key_to_token_mapping) {
                if (device.get_device_type() == FPGA_DEVICE) {
                    fpga_unique_key_to_token_map.insert(std::make_pair(device.get_pci_address(), token));
                }
            }

            devices.emplace_back(std::move(device));
        }
        catch (const OpaeppError& ex) {
            log_error("opaepp", "Could not read OPAE device: " << ex.what());
        }

        result = fpgaofDestroyToken(m_proxy_context->get_backend(), &token);
        OpaeApiError::throw_if_unexpected_result(result, "fpgaofDestroyToken", "Could not destroy token");
    }

    if (properties) {

        result = fpgaDestroyProperties(&properties);
        OpaeApiError::throw_if_unexpected_result(result, "fpgaDestroyProperties", "Could not destroy properties");
    }

    if (key_to_token_mapping) {
        m_proxy_context->set_enumerated_devices_mapping(fpga_unique_key_to_token_map);
    }

    return devices;
}


void OpaeProxyDeviceReader::read_properties(Device& device, fpga_token& token) {

    fpga_properties properties;

    auto result = fpgaofGetProperties(m_proxy_context->get_backend(), token, &properties);
    OpaeApiError::throw_if_unexpected_result(result, "fpgaofGetProperties", "Could not read device properties");

    PropertyContext context(m_proxy_context->get_backend(), &properties);

    read<fpga_objtype>(device.m_device_type, fpgaofPropertiesGetObjectType, context, "type");
    read<uint8_t>(device.m_bus, fpgaofPropertiesGetBus, context, "bus");
    read<uint8_t>(device.m_device, fpgaofPropertiesGetDevice, context, "device");
    read<uint8_t>(device.m_function, fpgaofPropertiesGetFunction, context, "function");
    read<uint8_t>(device.m_socket_id, fpgaofPropertiesGetSocketID, context, "socket id");
    read_optional<uint16_t>(device.m_device_id, fpgaofPropertiesGetDeviceID, context, "device id");
    read<fpga_guid>(device.m_guid, fpgaofPropertiesGetGUID, context, "GUID");
    read<uint64_t>(device.m_object_id, fpgaofPropertiesGetObjectID, context, "object id");

    if (device.m_device_type == FPGA_DEVICE) {
        read_optional<uint32_t>(device.m_num_slots, fpgaofPropertiesGetNumSlots, context, "number of slots");
        read_optional<uint64_t>(device.m_blue_bitstream_id, fpgaofPropertiesGetBBSID, context, "blue bitstream id");
        read_optional<fpga_version>(device.m_blue_bitstream_version, fpgaofPropertiesGetBBSVersion, context,
                                    "blue bitstream version");
        read_optional<uint16_t>(device.m_vendor_id, fpgaofPropertiesGetVendorID, context, "vendor id");
        read_optional_string(device.m_model, fpgaofPropertiesGetModel, context, "model");
        read_optional<uint64_t>(device.m_local_memory_size, fpgaofPropertiesGetLocalMemorySize, context,
                                "local memory size");
        read_optional<uint64_t>(device.m_capabilities, fpgaofPropertiesGetCapabilities, context, "capabilities");
    }
    else if (device.m_device_type == FPGA_ACCELERATOR) {
        read_optional<uint32_t>(device.m_mmio_spaces, fpgaofPropertiesGetNumMMIO, context, "mmio spaces");
        read_optional<uint32_t>(device.m_num_interrupts, fpgaofPropertiesGetNumInterrupts, context,
                                "number of interrupts");
        read_optional<fpga_accelerator_state>(device.m_accelerator_state, fpgaofPropertiesGetAcceleratorState, context,
                                              "accelerator state");
    }
    else {
        throw OpaeppError("Unrecognized device type");
    }

    fpgaDestroyProperties(&properties);
    OpaeApiError::throw_if_unexpected_result(result, "fpgaDestroyProperties");
}
