/*!
 * @brief FPGA-oF Processor discoverer implementation
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
 * @file fpgaof_processor_discoverer.cpp
 */

#include "agent-framework/module/managers/utils/manager_utils.hpp"
#include "agent-framework/module/utils/to_hex_string.hpp"
#include "uuid/uuid.hpp"
#include "discovery/builders/fpgaof_processor_builder.hpp"
#include "discovery/discoverers/fpgaof_processor_discoverer.hpp"
#include "discovery/builders/fpgaof_manager_builder.hpp"
#include "opaepp/opae-proxy/opae_proxy_device_reader.hpp"



using namespace agent::fpgaof::discovery;
using namespace agent::fpgaof::discovery::builders;
using namespace agent_framework::model;
using namespace agent_framework::model::attribute;
using namespace agent_framework::model::utils;

namespace {
std::string bbs_version_to_string(const OptionalField<fpga_version>& bbs_version) {
    if (bbs_version.has_value()) {

        std::stringstream bbs_version_ss;
        bbs_version_ss << static_cast<uint32_t>(bbs_version.value().major) << "."
                       << static_cast<uint32_t>(bbs_version.value().minor) << "."
                       << static_cast<uint32_t>(bbs_version.value().patch);

        return bbs_version_ss.str();
    }
    else {
        return std::string{};
    }
}


Array<FpgaReconfigurationSlot> get_reconfiguration_slots(const opaepp::Device& opae_device) {
    std::string uuid_str = unparse_uuid((const void*) opae_device.get_guid());

    FpgaReconfigurationSlot slot;
    slot.set_uuid(uuid_str);
    slot.set_programmable_from_host(true);
    Array<FpgaReconfigurationSlot> slots_array{slot};
    return slots_array;
}


OptionalField<std::string> optional_uint16_to_hex(const OptionalField<uint16_t>& optional_value) {
    if (optional_value.has_value()) {
        return to_hex_string<2, true>(optional_value.value());
    }
    else {
        return OptionalField<std::string>();
    }
}


void update_processor_with_opae(Processor& processor, const opaepp::Device& opae_device) {

    attribute::Fpga fpga = processor.get_fpga();

    if (opae_device.get_device_type() == FPGA_DEVICE) {

        fpga.set_firmware_version(::bbs_version_to_string(opae_device.get_blue_bitstream_version()));
        fpga.set_firmware_id(optional_uint16_to_hex(opae_device.get_blue_bitstream_id()));
        fpga.set_firmware_manufacturer(optional_uint16_to_hex(opae_device.get_vendor_id()));
        fpga.set_type(enums::FpgaType::Discrete);
        fpga.set_pcie_virtual_functions(0);
        fpga.set_bus(opae_device.get_bus());
        fpga.set_device(opae_device.get_device());
        fpga.set_function(opae_device.get_function());

        processor.set_manufacturer(optional_uint16_to_hex(opae_device.get_vendor_id()));
        processor.set_processor_type(enums::ProcessorType::FPGA);
    }

    if (opae_device.get_device_type() == FPGA_ACCELERATOR) {

        fpga.set_reconfiguration_slots(::get_reconfiguration_slots(opae_device));
    }

    processor.set_fpga(fpga);
}
}


std::vector<Processor> FpgaofProcessorDiscoverer::discover(const Uuid& system_uuid) {
    std::vector<Processor> processors{};

    auto opae_devices = m_context->opae_proxy_device_reader->get_devices();

    std::map<std::string, Processor> processor_cache{};

    for (const auto& opae_device: opae_devices) {

        std::string pci_address_key = opae_device.get_pci_address();

        if (processor_cache.find(pci_address_key) == processor_cache.end()) {

            processor_cache[pci_address_key] = FpgaofProcessorBuilder::build_default(system_uuid);
        }

        ::update_processor_with_opae(processor_cache[pci_address_key], opae_device);
    }

    processors.reserve(processor_cache.size());

    std::transform(processor_cache.begin(), processor_cache.end(), std::back_inserter(processors),
                   [](auto const& pair) { return std::move(pair.second); });

    return processors;
}