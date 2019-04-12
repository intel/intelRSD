/*!
 * @brief Processor builder class implementation.
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
 * @file processor_builder.cpp
 */


#include "discovery/builders/processor_builder.hpp"


#include <bitset>



using namespace agent::compute::discovery;
using namespace smbios::parser;
using namespace agent_framework::model;

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
    static const char* feature_flag_values[FLAGS_COUNT] = {
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

    for (int i = 0; i < FLAGS_COUNT; ++i) {
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
    log_debug("smbios-discovery", "Processor status: 0x" << std::hex << int(status));
    log_debug("smbios-discovery", "Processor socket populated: "
        << (is_socket_populated ? "true." : "false."));
    return is_socket_populated;
}


std::string cpu_id_to_string(const std::uint8_t data[REGISTER_WIDTH]) {
    std::stringstream ss{};
    ss << "0x";
    ss << std::uppercase << std::hex;
    for (int i = 0; i < REGISTER_WIDTH; ++i) {
        ss << std::setfill('0') << std::setw(2) << int(data[i]);
    }
    return ss.str();
}


void read_socket_index_from_string(const std::string& socket, std::string& index) {
    static const constexpr char CPU_PREFIX[] = "CPU";
    static const constexpr char SOCKET_PREFIX[] = "Socket ";

    if (socket.rfind(CPU_PREFIX, 0) == 0) {
        index = socket.substr(strlen(CPU_PREFIX));
        log_debug("smbios-discovery", "Socket index = " << index << " found in " << socket);
    }
    else if (socket.rfind(SOCKET_PREFIX, 0) == 0) {
        index = socket.substr(strlen(SOCKET_PREFIX));
        log_debug("smbios-discovery", "Socket index = " << index << " found in " << socket);
    }
    else {
        log_warning("smbios-discovery", "Unrecognized format of socket!");
        index = {};
    }
}


/*!
 * @brief Check if processor socket designation match CPU ID socket designation
 *
 * Due to defect in SMBIOS, socket designation can be "CPUx" or "Socket x" (x is index of processor).
 * This function parses socket designation from both sources and checks only processor index.
 *
 * @param[in] processor_socket Socket designation read from processor SMBIOS data
 * @param[in] cpu_id_socket Socket designation read from CPU ID SMBIOS data
 *
 * @return True if both sockets point to the same processor, false otherwise.
 */
bool is_matching_processor(const std::string& processor_socket, const std::string& cpu_id_socket) {
    if (processor_socket == cpu_id_socket) {
        return true;
    }

    std::string processor_socket_index{};
    std::string cpu_id_socket_index{};

    read_socket_index_from_string(processor_socket, processor_socket_index);
    read_socket_index_from_string(cpu_id_socket, cpu_id_socket_index);

    return (processor_socket_index == cpu_id_socket_index);
}


template<typename SmbiosFpgaData>
OptionalField<enums::FpgaType> get_fpga_type(const SmbiosFpgaData& fpga) {
    switch (fpga.data.fpga_type) {
        case FpgaType::INTEGRATED:
            return enums::FpgaType::Integrated;
        case FpgaType::DISCRETE:
            return enums::FpgaType::Discrete;
        case FpgaType::DISCRETE_WITH_SOC:
            /* fallthrough */
        default:
            return OptionalField<enums::FpgaType> {};
    }
}


template<typename SmbiosFpgaData>
OptionalField<enums::IntegratedMemoryType>
get_processor_memory_type(const SmbiosFpgaData& fpga) {
    switch (fpga.data.memory_technology) {
        case IntegratedMemoryTechnology::EDRAM:
            return enums::IntegratedMemoryType::EDRAM;
        case IntegratedMemoryTechnology::HBM:
            return enums::IntegratedMemoryType::HBM;
        case IntegratedMemoryTechnology::HBM2:
            return enums::IntegratedMemoryType::HBM2;
        case IntegratedMemoryTechnology::NONE:
        default:
            return OptionalField<enums::IntegratedMemoryType>{};

    }
}


template<typename SmbiosFpgaData>
OptionalField<attribute::Status> get_fpga_status(const SmbiosFpgaData& fpga) {
    switch (fpga.data.fpga_status) {
        case FpgaStatus::ENABLED:
            return attribute::Status{enums::State::Enabled, {}};
        case FpgaStatus::DISABLED:
        default:
            return attribute::Status{enums::State::Disabled, {}};
    }
}


/*!
* @tparam SmbiosFpgaData Type of structure containing SMBIOS FPGA data.
* @brief Update processor object with SMBIOS FPGA data.
* @param processor Processor object to be filled with discovered data.
* @param smbios_data SMBIOS data.
*/
template<typename SmbiosFpgaData>
static void generic_update_smbios_fpga_data(agent_framework::model::Processor& processor,
                                            const SmbiosFpgaData& smbios_data) {
    attribute::Fpga fpga{};
    fpga.set_type(get_fpga_type(smbios_data));
    attribute::CpuId cpu_id{};
    cpu_id.set_vendor_id(smbios_data.get_string(smbios_data.data.fpga_vendor));
    cpu_id.set_family(smbios_data.get_string(smbios_data.data.fpga_family));

    attribute::IntegratedMemory memory{};
    memory.set_type(get_processor_memory_type(smbios_data));
    memory.set_capacity_mb(smbios_data.data.integrated_memory_capacity);
    memory.set_speed_mhz(smbios_data.data.integrated_memory_speed);

    processor.set_total_cores(smbios_data.data.fpga_hps_core_count);
    processor.set_model_name(smbios_data.get_string(smbios_data.data.fpga_model));
    processor.set_tdp_watt(smbios_data.data.thermal_design_power);
    processor.set_status(get_fpga_status(smbios_data));
    processor.set_socket(std::to_string(int(smbios_data.data.socket_identifier)));
    processor.set_processor_type(enums::ProcessorType::FPGA);
    processor.set_processor_architecture(enums::ProcessorArchitecture::OEM);

    processor.set_fpga(fpga);
    processor.set_cpu_id(cpu_id);
    processor.add_memory(memory);
}

}


Processor ProcessorBuilder::build_default(const std::string& parent_uuid) {
    Processor processor{parent_uuid};
    processor.add_collection({enums::CollectionName::Processors, enums::CollectionType::Processors});
    return processor;
}


void ProcessorBuilder::update_smbios_processor_data(agent_framework::model::Processor& processor,
                                                    const smbios::parser::SmbiosParser::StructEnhanced<smbios::parser::SMBIOS_PROCESSOR_INFO_DATA>& cpu) {

    const constexpr int WORD = 16;
    const constexpr int CAPABILITY_64_BIT_INDEX = 2;
    const std::map<ProcessorType, OptionalField<enums::ProcessorType>> processor_types = {
        {ProcessorType::NOT_SET, {}}, // Null if not set
        {ProcessorType::CENTRAL, enums::ProcessorType::CPU},
        {ProcessorType::DSP,     enums::ProcessorType::DSP},
        {ProcessorType::VIDEO,   enums::ProcessorType::GPU},
        {ProcessorType::OTHER,   enums::ProcessorType::OEM},
        {ProcessorType::MATH,    enums::ProcessorType::OEM},
        {ProcessorType::UNKNOWN, enums::ProcessorType::OEM}
    };
    attribute::Status status{};

    processor.set_socket(cpu.get_string(cpu.data.socket_designation));
    log_debug("smbios-discovery", "Discovering Processor (" << processor.get_socket() << ").");

    if (is_cpu_present(cpu.data)) {

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
            log_error("smbios-discovery", "Processor parsing exception: " << ex.what());
            // Set default if we could not parse processor type
            processor.set_processor_type(enums::ProcessorType::OEM);
        }

        processor.set_manufacturer(cpu.get_string(cpu.data.processor_manufacturer));

        std::bitset<WORD> processor_characteristics(cpu.data.processor_characteristics);
        bool is_capable_64_bit = processor_characteristics[CAPABILITY_64_BIT_INDEX];

        processor.set_instruction_set(
            is_capable_64_bit ? enums::ProcessorInstructionSet::x86_64 : enums::ProcessorInstructionSet::x86);

        // Finding model (processor brand) in substring of processor version
        for (const auto& model : enums::ProcessorModel::get_values()) {
            std::size_t found = processor_version.find(model);
            if (found != std::string::npos) {
                processor.set_model(enums::ProcessorModel::from_string(model));
                break;
            }
        }

        set_cpu_id(processor, cpu.data);
        set_capabilities(processor, cpu.data);
    }
    else {
        log_warning("smbios-discovery", "Processor (" << processor.get_socket() << ") is not present.");
        status.set_state(enums::State::Absent);
        status.set_health({}); // Set health to null
    }

    processor.set_status(status);
}


void ProcessorBuilder::update_smbios_cpu_id_data(agent_framework::model::Processor& processor,
                                                 const std::vector<smbios::parser::SmbiosParser::StructEnhanced<smbios::parser::SMBIOS_CPUID_DATA>>& cpu_ids_v1,
                                                 const std::vector<smbios::parser::SmbiosParser::StructEnhanced<smbios::parser::SMBIOS_CPUID_DATA_V2>>& cpu_ids_v2) {
    log_debug("smbios-discovery", "Discovering CPU ID for processor " << processor.get_socket());
    attribute::ExtendedCpuId cpu_id{};

    bool cpu_id_v1_found = false;
    for (const auto& cpu_id_v1 : cpu_ids_v1) {
        // Matching processor by socket designation
        if (::is_matching_processor(processor.get_socket(), cpu_id_v1.get_string(cpu_id_v1.data.socket_designation))) {
            log_debug("smbios-discovery", "Found CPU ID (Subtype 1) record for processor " << processor.get_socket());

            cpu_id.set_eax_00h(::cpu_id_to_string(cpu_id_v1.data.eax_00));
            cpu_id.set_eax_01h(::cpu_id_to_string(cpu_id_v1.data.eax_01));
            cpu_id.set_eax_02h(::cpu_id_to_string(cpu_id_v1.data.eax_02));
            cpu_id.set_eax_03h(::cpu_id_to_string(cpu_id_v1.data.eax_03));
            cpu_id.set_eax_04h(::cpu_id_to_string(cpu_id_v1.data.eax_04));
            cpu_id.set_eax_05h(::cpu_id_to_string(cpu_id_v1.data.eax_05));
            cpu_id.set_eax_06h(::cpu_id_to_string(cpu_id_v1.data.eax_06));
            cpu_id.set_eax_07h(::cpu_id_to_string(cpu_id_v1.data.eax_07));
            cpu_id.set_eax_09h(::cpu_id_to_string(cpu_id_v1.data.eax_09));
            cpu_id.set_eax_0ah(::cpu_id_to_string(cpu_id_v1.data.eax_0a));
            cpu_id.set_eax_0bh(::cpu_id_to_string(cpu_id_v1.data.eax_0b));
            cpu_id.set_eax_0dh(::cpu_id_to_string(cpu_id_v1.data.eax_0d));
            cpu_id.set_eax_0fh(::cpu_id_to_string(cpu_id_v1.data.eax_0f));
            cpu_id.set_eax_10h(::cpu_id_to_string(cpu_id_v1.data.eax_10));

            cpu_id_v1_found = true;
        }
    }

    if (!cpu_id_v1_found) {
        log_warning("smbios-discovery",
                    "CPU ID (Subtype 1) for processor " << processor.get_socket() << " not discovered");
    }

    bool cpu_id_v2_found = false;
    for (const auto& cpu_id_v2 : cpu_ids_v2) {
        // Matching processor by socket designation
        if (::is_matching_processor(processor.get_socket(), cpu_id_v2.get_string(cpu_id_v2.data.socket_designation))) {
            log_debug("smbios-discovery", "Found CPU ID (Subtype 2) record for processor " << processor.get_socket());

            cpu_id.set_eax_14h(::cpu_id_to_string(cpu_id_v2.data.eax_14));
            cpu_id.set_eax_15h(::cpu_id_to_string(cpu_id_v2.data.eax_15));
            cpu_id.set_eax_16h(::cpu_id_to_string(cpu_id_v2.data.eax_16));
            cpu_id.set_eax_17h_ecx_00h(::cpu_id_to_string(cpu_id_v2.data.eax_17_00));
            cpu_id.set_eax_17h_ecx_01h(::cpu_id_to_string(cpu_id_v2.data.eax_17_01));
            cpu_id.set_eax_17h_ecx_02h(::cpu_id_to_string(cpu_id_v2.data.eax_17_02));
            cpu_id.set_eax_17h_ecx_03h(::cpu_id_to_string(cpu_id_v2.data.eax_17_03));
            cpu_id.set_eax_80000000h(::cpu_id_to_string(cpu_id_v2.data.eax_80000000));
            cpu_id.set_eax_80000001h(::cpu_id_to_string(cpu_id_v2.data.eax_80000001));
            cpu_id.set_eax_80000002h(::cpu_id_to_string(cpu_id_v2.data.eax_80000002));
            cpu_id.set_eax_80000003h(::cpu_id_to_string(cpu_id_v2.data.eax_80000003));
            cpu_id.set_eax_80000004h(::cpu_id_to_string(cpu_id_v2.data.eax_80000004));
            cpu_id.set_eax_80000006h(::cpu_id_to_string(cpu_id_v2.data.eax_80000006));
            cpu_id.set_eax_80000007h(::cpu_id_to_string(cpu_id_v2.data.eax_80000007));
            cpu_id.set_eax_80000008h(::cpu_id_to_string(cpu_id_v2.data.eax_80000008));

            cpu_id_v2_found = true;
        }
    }

    if (!cpu_id_v2_found) {
        log_warning("smbios-discovery",
                    "CPU ID (Subtype 2) for processor " << processor.get_socket() << " not discovered");
    }

    processor.set_extended_cpu_id(cpu_id);
}


void ProcessorBuilder::update_smbios_fpga_data(agent_framework::model::Processor& processor,
                                               const smbios::parser::SmbiosParser::StructEnhanced<SMBIOS_FPGA_DATA>& smbios_data) {
    generic_update_smbios_fpga_data(processor, smbios_data);
}


void ProcessorBuilder::update_smbios_fpga_data(agent_framework::model::Processor& processor,
                                               const smbios::parser::SmbiosParser::StructEnhanced<SMBIOS_FPGA_DATA_OEM>& smbios_data) {
    generic_update_smbios_fpga_data(processor, smbios_data);
}