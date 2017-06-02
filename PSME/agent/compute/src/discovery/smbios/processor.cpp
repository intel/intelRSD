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
 * @file processor.cpp
 * @brief SMBIOS-way of discovering processor properties
 * */

#include "agent-framework/module/compute_components.hpp"
#include "discovery/smbios/processor.hpp"
#include <bitset>

using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace smbios::parser;

namespace {

void set_cpu_id(Processor& processor, const PROCESSOR_INFO_DATA& cpu) {

    // For explanation see '7.5.3.1 x86-class CPUs - System Management BIOS (SMBIOS) Reference Specification
    uint32_t eax = cpu.processor_id.eax;

    /*
     *
     * For now ignore checking if processor is manufactured by Intel. If we do check, we risk that we do not fill in
     * cpu_id for new, unknown Intel CPU
     *
     * uint8_t cpu_family = static_cast<uint8_t>(cpu.processor_family);
     * bool is_intel = (cpu_family < 0x16) || (cpu_family >= 0x28 && cpu_family <= 0x2c) || (cpu_family == 0x82)
     *              || (cpu_family >= 0xa1 && cpu_family < 0xb4) || (cpu_family == 0xb5)
     *              || (cpu_family >= 0xb8 && cpu_family <= 0xbd) || (cpu_family >= 0xbf && cpu_family <= 0xc7)
     *              || (cpu_family >= 0xcd && cpu_family <= 0xce) || (cpu_family >= 0xd6 && cpu_family <= 0xd8)
     *              || (cpu_family >= 0xda && cpu_family <= 0xdb) || (cpu_family >= 0xdd && cpu_family <= 0xe0);
     */

    bool is_intel = true;

    if (is_intel) {
        attribute::CpuId cpu_id{};

        cpu_id.set_model(std::to_string(((eax & 0x000f0000) >> 12) + ((eax & 0x000000f0) >> 4)));
        cpu_id.set_family(std::to_string(((eax & 0x0ff00000) >> 20) + ((eax & 0x00000f00) >> 8)));
        cpu_id.set_step(std::to_string(eax & 0xf));

        const uint8_t* processor_id = reinterpret_cast<const uint8_t*>(&cpu.processor_id);
        char id[] = "xx xx xx xx xx xx xx xx";
        std::snprintf(id, strlen(id) + 1, "%02x %02x %02x %02x %02x %02x %02x %02x",
                      processor_id[0], processor_id[1], processor_id[2], processor_id[3],
                      processor_id[4], processor_id[5], processor_id[6], processor_id[7]);
        cpu_id.set_numeric_id(id);
        //cpu_id.set_microcode_info(); // msr not available
        //cpu_id.set_vendor_id(); // not possible

        processor.set_cpu_id(cpu_id);
    }
}

void set_capabilities(Processor& processor, const PROCESSOR_INFO_DATA& cpu) {
    static const int FLAGS_COUNT = 32;
    static const char *feature_flag_values[FLAGS_COUNT] = {
            "FPU",
            "VME",
            "DE",
            "PSE",
            "TSC",
            "MSR",
            "PAE",
            "MCE",
            "CX8",
            "APIC",
            nullptr,
            "SEP",
            "MTRR",
            "PGE",
            "MCA",
            "CMOV",
            "PAT",
            "PSE-36",
            "PSN",
            "CLFSH",
            nullptr,
            "DS",
            "ACPI",
            "MMX",
            "FXSR",
            "SSE",
            "SSE2",
            "SS",
            "HTT",
            "TM",
            nullptr,
            "PBE"
    };

    std::vector<std::string> capabilities{};

    for (int i=0;i < FLAGS_COUNT; ++i) {
        if (feature_flag_values[i] != nullptr && cpu.processor_id.edx & (1 << i)) {
            capabilities.push_back(feature_flag_values[i]);
        }
    }
    processor.set_capabilities(capabilities);

}

bool is_cpu_present(const PROCESSOR_INFO_DATA& cpu) {
    static const constexpr int CPU_SOCKET_POPULATED_MASK = 0x40;
    const auto& status = cpu.status;
    const auto cpu_socket_populated_bit = status & CPU_SOCKET_POPULATED_MASK;
    const auto is_socket_populated = cpu_socket_populated_bit != 0;
    log_debug(GET_LOGGER("smbios-discovery"), "Processor status: 0x" << std::hex << int(status));
    log_debug(GET_LOGGER("smbios-discovery"), "Processor socket populated: "
        << (is_socket_populated ? "true." : "false."));
    return is_socket_populated;
}

}

namespace agent {
namespace compute {
namespace discovery {
namespace smbios {

std::tuple<bool, enums::Health> discover_processor(SmbiosParser::Ptr smbios, System& system) {
    if (!smbios) {
        return std::make_tuple(false, enums::Health::OK);
    }

    const constexpr int WORD = 16;
    const constexpr int CAPABILITY_64_BIT_INDEX = 2;
    const std::map<ProcessorType, OptionalField<enums::ProcessorType>> processor_types = {
        {ProcessorType::NOT_SET, {}}, // Null if not set
        {ProcessorType::CENTRAL, enums::ProcessorType::CPU},
        {ProcessorType::DSP, enums::ProcessorType::DSP},
        {ProcessorType::VIDEO, enums::ProcessorType::GPU},
        {ProcessorType::OTHER, enums::ProcessorType::OEM},
        {ProcessorType::MATH, enums::ProcessorType::OEM},
        {ProcessorType::UNKNOWN, enums::ProcessorType::OEM}
    };

    auto smbios_processors = smbios->get_all<SMBIOS_PROCESSOR_INFO_DATA>();
    for (const auto& cpu : smbios_processors) {
        attribute::Status status{};
        Processor processor{system.get_uuid()};
        processor.set_socket(cpu.get_string(cpu.data.socket_designation));
        log_debug(GET_LOGGER("smbios-discovery"), "Discovering Processor (" << processor.get_socket() << ").");

        if(is_cpu_present(cpu.data)) {
            // Basic discovery:
            std::string processor_version = cpu.get_string(cpu.data.processor_version);
            processor.set_model_name(processor_version);
            processor.set_max_speed_mhz(std::uint32_t(cpu.data.max_speed_mhz));
            status.set_state(enums::State::Enabled);

            // Deep discovery:
            const auto total_cores = std::uint32_t(cpu.data.core_count);
            const auto enabled_cores = std::uint32_t(cpu.data.core_enabled);
            const auto total_threads = std::uint32_t(cpu.data.thread_count);

            processor.set_total_cores(total_cores);
            processor.set_total_threads(total_threads);
            processor.set_enabled_cores(enabled_cores);

            // Sanity check to ensure that floating point exception will not be thrown
            if (total_cores > 0u) {
                // Division is well defined
                const auto enabled_threads = enabled_cores * (total_threads / total_cores);
                processor.set_enabled_threads(enabled_threads);
            }

            try {
                processor.set_processor_type(processor_types.at(cpu.data.processor_type));
            }
            catch (const std::exception& ex) {
                log_error(GET_LOGGER("smbios-discovery"), "Processor parsing exception: " << ex.what());
                // Set default if we could not parse processor type
                processor.set_processor_type(enums::ProcessorType::OEM);
            }

            processor.set_manufacturer(cpu.get_string(cpu.data.processor_manufacturer));

            std::bitset<WORD> processor_characteristics(cpu.data.processor_characteristics);
            bool is_capable_64_bit = processor_characteristics[CAPABILITY_64_BIT_INDEX];

            processor.set_instruction_set(
                is_capable_64_bit ? enums::ProcessorInstructionSet::x86_64 : enums::ProcessorInstructionSet::x86);

            // Finding model (processor brand) in substring of processor version
            enums::ProcessorModel processor_model{enums::ProcessorModel::Unknown};
            for (const auto& model : enums::ProcessorModel::get_values()) {
                std::size_t found = processor_version.find(model);
                if (found != std::string::npos) {
                    processor_model = enums::ProcessorModel::from_string(model);
                    break;
                }
            }
            processor.set_model(processor_model);

            set_cpu_id(processor, cpu.data);
            set_capabilities(processor, cpu.data);
        }
        else {
            log_warning(GET_LOGGER("smbios-discovery"), "Processor (" << processor.get_socket() << ") is not present.");
            status.set_state(enums::State::Absent);
            status.set_health({}); // Set health to null
        }

        processor.set_status(status);
        ComputeComponents::get_instance()->get_processor_manager().add_entry(std::move(processor));
    }

    log_info(GET_LOGGER("smbios-discovery"), "Processor Discovery was successful.");
    return std::make_tuple(true, enums::Health::OK);
}

}
}
}
}
