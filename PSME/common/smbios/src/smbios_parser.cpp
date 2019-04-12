/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 *
 * @file smbios_parser.cpp
 *
 * @brief Smbios parser implementation
 * */

#include "smbios/smbios_parser.hpp"
#include "logger/logger.hpp"
#include <numeric>
#include <cstddef>
#include <cstring>
#include <ostream>
#include <iomanip>
#include <iterator>
#include <safe-string/safe_lib.hpp>
#include <bitset>



namespace smbios {
namespace parser {

template<>
bool is_valid_structure_length<SMBIOS_SPEED_SELECT_INFO_DATA>(const uint8_t length) {
    static constexpr uint8_t MIN_LENGTH = sizeof(SMBIOS_SPEED_SELECT_INFO_DATA);
    // variable length structure: contains zero or more Speed Select Configurations
    return length >= MIN_LENGTH &&
        (length - MIN_LENGTH) % sizeof(SPEED_SELECT_CONFIGURATION) == 0;
}

SmbiosParser::Exception::~Exception() {}

}
}

namespace mdr {

template<>
StructEnhanced<smbios::parser::SMBIOS_SPEED_SELECT_INFO_DATA> parse_struct<smbios::parser::SMBIOS_SPEED_SELECT_INFO_DATA>(
    std::shared_ptr<const uint8_t> bufp, const size_t buf_size, uint64_t& offset, bool has_auxiliary_string) {
    using namespace smbios::parser;

    auto buf = bufp.get();
    const SMBIOS_SPEED_SELECT_INFO_DATA* structure = reinterpret_cast<const SMBIOS_SPEED_SELECT_INFO_DATA*>(buf + offset);
    std::vector<SPEED_SELECT_CONFIGURATION> configs;
    std::vector<std::string> strings{};

    offset += sizeof(SMBIOS_SPEED_SELECT_INFO_DATA);

    // Verify length of configs in buffer. Skip reading them if there is a mismatch.
    if ((structure->header.length - sizeof(SMBIOS_SPEED_SELECT_INFO_DATA)) / sizeof(SPEED_SELECT_CONFIGURATION)
        != structure->data.number_of_configs) {

        log_warning("smbios", "SMBIOS Speed Select structure: length and number of configurations do not match for handle "
                    << structure->header.handle << ". Speed Select Configurations skipped!");
        offset += structure->header.length - sizeof(SMBIOS_SPEED_SELECT_INFO_DATA);
    }
    else {
        for (uint8_t i = 0; i < structure->data.number_of_configs; ++i) {
            const SPEED_SELECT_CONFIGURATION config = *reinterpret_cast<const SPEED_SELECT_CONFIGURATION*>(buf + offset);
            configs.push_back(config);
            offset += sizeof(SPEED_SELECT_CONFIGURATION);
        }
    }

    if (has_auxiliary_string) {
        read_auxiliary_strings(buf, buf_size, offset, strings);
    }

    return {*structure, std::move(configs), std::move(strings), bufp};
}


template<>
std::shared_ptr<const uint8_t> smbios::parser::SmbiosParser::init_buffer(const uint8_t* _buf, size_t _buf_size) {
    if (nullptr == _buf) {
        throw Exception("Invalid SMBIOS blob pointer");
    }
    auto buffer = std::shared_ptr<const uint8_t>(new uint8_t[_buf_size], std::default_delete<uint8_t[]>());
    std::copy(_buf, _buf + _buf_size, const_cast<uint8_t*>(buffer.get()));
    return buffer;
}


template<>
std::ostream& operator<<(std::ostream& os, const smbios::parser::SmbiosParser& parser) {
    using namespace smbios::parser;

    os
        << "SMBIOS:\n"
        << "\tVersion: " << parser.entry_point->get_major_version() << "."
        << parser.entry_point->get_minor_version() << std::endl
        << "\tStructure Table Address: 0x" << std::hex << parser.entry_point->get_struct_table_address() << std::endl;

    auto offset = parser.entry_point->get_struct_table_address();
    while (offset + sizeof(SMBIOS_OOB_HEADER) < parser.buf_size) {
        os << "offset : " << std::dec << offset << std::endl;
        const SMBIOS_OOB_HEADER& header = *reinterpret_cast<const SMBIOS_OOB_HEADER*>(parser.buf.get() + offset);
        if (offset + header.length > parser.buf_size) {
            throw SmbiosParser::Exception("Unexpectedly reached end of smbios blob");
        }
        switch (header.type) {
            case SMBIOS_BIOS_INFO_DATA::ID: {
                const auto data = parser.read_struct<SMBIOS_BIOS_INFO_DATA>(offset);
                os << data;
                break;
            }
            case SMBIOS_SYSTEM_INFO_DATA::ID: {
                const auto data = parser.read_struct<SMBIOS_SYSTEM_INFO_DATA>(offset);
                os << data;
                break;
            }
            case SMBIOS_MODULE_INFO_DATA::ID: {
                const auto data = parser.read_struct<SMBIOS_MODULE_INFO_DATA>(offset);
                os << data;
                break;
            }
            case SMBIOS_NIC_INFO_DATA::ID: {
                if (sizeof(SMBIOS_NIC_INFO_DATA) == header.length) {
                    const auto data = parser.read_struct<SMBIOS_NIC_INFO_DATA>(offset);
                    os << data;
                }
                else if (sizeof(SMBIOS_NIC_INFO_DATA_V2) == header.length) {
                    const auto data = parser.read_struct<SMBIOS_NIC_INFO_DATA_V2>(offset);
                    os << data;
                }
                break;
            }
            case SMBIOS_STORAGE_INFO_DATA::ID: {
                if (sizeof(SMBIOS_STORAGE_INFO_DATA) == header.length) {
                    const auto data = parser.read_struct<SMBIOS_STORAGE_INFO_DATA>(offset);
                    os << data;
                }
                else if (sizeof(SMBIOS_STORAGE_INFO_DATA_V2) == header.length) {
                    const auto data = parser.read_struct<SMBIOS_STORAGE_INFO_DATA_V2>(offset);
                    os << data;
                }
                break;
            }
            case SMBIOS_PROCESSOR_INFO_DATA::ID: {
                const auto data = parser.read_struct<SMBIOS_PROCESSOR_INFO_DATA>(offset);
                os << data;
                break;
            }
            case SMBIOS_CPUID_DATA::ID: {
                if (sizeof(SMBIOS_CPUID_DATA) == header.length) {
                    const auto data = parser.read_struct<SMBIOS_CPUID_DATA>(offset);
                    os << data;
                }
                else if (sizeof(SMBIOS_CPUID_DATA_V2) == header.length) {
                    const auto data = parser.read_struct<SMBIOS_CPUID_DATA_V2>(offset);
                    os << data;
                }
                break;
            }
            case SMBIOS_PCIE_INFO_DATA::ID: {
                const auto data = parser.read_struct<SMBIOS_PCIE_INFO_DATA>(offset);
                os << data;
                break;
            }
            case SMBIOS_FPGA_DATA::ID: {
                if (sizeof(SMBIOS_FPGA_DATA) == header.length) {
                    const auto data = parser.read_struct<SMBIOS_FPGA_DATA>(offset);
                    os << data;
                }
                else if (sizeof(SMBIOS_FPGA_DATA_OEM) == header.length) {
                    const auto data = parser.read_struct<SMBIOS_FPGA_DATA_OEM>(offset);
                    os << data;
                }
                break;
            }
            case SMBIOS_STORAGE_DEVICE_INFO_DATA::ID: {
                const auto data = parser.read_struct<SMBIOS_STORAGE_DEVICE_INFO_DATA>(offset);
                os << data;
                break;
            }
            case SMBIOS_TPM_INFO_DATA::ID: {
                const auto data = parser.read_struct<SMBIOS_TPM_INFO_DATA>(offset);
                os << data;
                break;
            }
            case SMBIOS_TXT_INFO_DATA::ID: {
                const auto data = parser.read_struct<SMBIOS_TXT_INFO_DATA>(offset);
                os << data;
                break;
            }
            case SMBIOS_MEMORY_DEVICE::ID: {
                const auto data = parser.read_struct<SMBIOS_MEMORY_DEVICE>(offset);
                os << data;
                break;
            }
            case SMBIOS_MEMORY_DEVICE_EXTENDED_INFO_DATA::ID: {
                const auto data = parser.read_struct<SMBIOS_MEMORY_DEVICE_EXTENDED_INFO_DATA>(offset);
                os << data;
                break;
            }
            case SMBIOS_PCIE_PORT_INFO_DATA::ID: {
                const auto data = parser.read_struct<SMBIOS_PCIE_PORT_INFO_DATA>(offset);
                os << data;
                break;
            }
            case SMBIOS_SPEED_SELECT_INFO_DATA::ID: {
                const auto data = parser.read_struct<SMBIOS_SPEED_SELECT_INFO_DATA>(offset);
                os << data;
                break;
            }
            default:
                offset += header.length;
                /* Get past trailing string-list - double-null */
                while (offset + 1 < parser.buf_size
                       && std::uint8_t(*(parser.buf.get() + offset + 1) | *(parser.buf.get() + offset)) != 0) {
                    offset++;
                }
                offset += 2;
                break;
        }
    }
    return os;
}


std::ostream& operator<<(std::ostream& os, const StructEnhanced<smbios::parser::SMBIOS_BIOS_INFO_DATA>& s) {
    std::string bios_characteristics = std::bitset<64>(s.data.characteristics).to_string();
    os << "SMBIOS_BIOS_INFO_DATA\t@" << s.header.handle << " " << std::dec << int(s.header.length) << "B" << "\n";
    os << "\tVendor                       : " << s.get_string(s.data.vendor)
       << "\n\tVersion                      : " << s.get_string(s.data.version)
       << "\n\tStarting address segment     : 0x" << std::hex << s.data.starting_address_segment
       << "\n\tRelease date                 : " << s.get_string(s.data.release_date)
       << "\n\tRom size                     : " << std::dec << int(s.data.rom_size) * 64 << "K"
       << "\n\tBIOS characteristics         : " << bios_characteristics
       << std::endl;
    return os;
}


std::ostream& operator<<(std::ostream& os, const StructEnhanced<smbios::parser::SMBIOS_SYSTEM_INFO_DATA>& s) {
    os << "SMBIOS_SYSTEM_INFO_DATA\t@" << s.header.handle << "\n";
    os << "\tManufacturer    : " << s.get_string(s.data.manufacturer)
       << "\n\tProduct Name    : " << s.get_string(s.data.product_name)
       << "\n\tVersion         : " << s.get_string(s.data.version)
       << "\n\tSerial Number   : " << s.get_string(s.data.serial_number)
       << "\n\tUUID            : ";
    for (uint8_t b : s.data.uuid) {
        os << std::hex << static_cast<uint16_t>(b);
    }
    os << "\n\tWake-up Type    : 0x" << std::hex << static_cast<uint32_t>(s.data.wakeup_type)
       << "\n\tSKU Number      : " << s.get_string(s.data.sku_number)
       << "\n\tFamily          : " << s.get_string(s.data.family)
       << std::endl;
    return os;
}


std::ostream& operator<<(std::ostream& os, const StructEnhanced<smbios::parser::SMBIOS_MODULE_INFO_DATA>& s) {
    os << "SMBIOS_MODULE_INFO_DATA\t@" << s.header.handle << "\n";
    os << "\tManufacturer    : " << s.get_string(s.data.manufacturer)
       << "\n\tProduct Name    : " << s.get_string(s.data.product)
       << "\n\tVersion         : " << s.get_string(s.data.version)
       << "\n\tSerial Number   : " << s.get_string(s.data.serial_number)
       << "\n\tAsset tag       : " << s.get_string(s.data.asset_tag)
       << "\n\tFeature flags   : " << std::bitset<8>(s.data.feature_flags).to_string()
       << "\n\tLocation in chassis : " << s.get_string(s.data.location_in_chassis)
       << "\n\tBoard type      : 0x" << std::hex << static_cast<uint32_t>(s.data.board_type) << std::endl;
    return os;
}


std::ostream& operator<<(std::ostream& os, const StructEnhanced<smbios::parser::SMBIOS_PCIE_INFO_DATA>& s) {
    os << "PCIE_INFO_DATA\t@" << s.header.handle << "\n";
    os << "\tPciClassCode    : 0x" << std::hex << static_cast<uint32_t>(s.data.ClassCode)
       << "\n\tSlotNo          : 0x" << std::hex << s.data.SlotNo
       << "\n\tVendorID        : 0x" << std::hex << s.data.VendorID
       << "\n\tDeviceID        : 0x" << std::hex << s.data.DeviceID
       << "\n\tSubVendorID     : 0x" << std::hex << s.data.SubVendorID
       << "\n\tSubDeviceID     : 0x" << std::hex << s.data.SubDeviceID
       << "\n\tLinkSpeed       : " << std::dec << s.data.LinkSpeed
       << "\n\tLinkWidth       : " << std::dec << s.data.LinkWidth
       << std::endl;
    return os;
}


std::ostream& operator<<(std::ostream& os, const StructEnhanced<smbios::parser::SMBIOS_PROCESSOR_INFO_DATA>& s) {
    os << "PROCESSOR INFO DATA\t@" << s.header.handle << "\n";
    os << "\tSocketDesignation        : " << s.get_string(s.data.socket_designation)
       << "\n\tProcessorType            : 0x" << std::hex << static_cast<int>(s.data.processor_type)
       << "\n\tProcessorFamily          : 0x" << std::hex << static_cast<int>(s.data.processor_family)
       << "\n\tProcessorManufacturer    : " << s.get_string(s.data.processor_manufacturer)
       << "\n\tProcessorVersion         : " << s.get_string(s.data.processor_version)
       << "\n\tVoltage                  : " << std::dec << int(s.data.voltage)
       << "\n\tExternalClock            : " << std::dec << int(s.data.external_clock)
       << "\n\tMaxSpeedMhz              : " << std::dec << int(s.data.max_speed_mhz)
       << "\n\tCurrentSpeed             : " << std::dec << int(s.data.current_speed)
       << "\n\tStatus                   : " << std::bitset<8>(s.data.status)
       << "\n\tProcessorUpdate          : 0x" << std::hex << int(s.data.processor_update)
       << "\n\tL1CacheHandle            : 0x" << std::hex << int(s.data.l1_cache_handle)
       << "\n\tL2CacheHandle            : 0x" << std::hex << int(s.data.l2_cache_handle)
       << "\n\tL3CacheHandle            : 0x" << std::hex << int(s.data.l3_cache_handle)
       << "\n\tSerialNumber             : " << s.get_string(s.data.serial_number)
       << "\n\tAssetTag                 : " << s.get_string(s.data.asset_tag)
       << "\n\tPartNumber               : " << s.get_string(s.data.part_number)
       << "\n\tCoreCount                : " << std::dec << int(s.data.core_count)
       << "\n\tCoreEnabled              : " << std::dec << int(s.data.core_enabled)
       << "\n\tThreadCount              : " << std::dec << int(s.data.thread_count)
       << "\n\tProcessorCharacteristics : " << std::dec << int(s.data.processor_characteristics)
       << "\n\tProcessorFamily2         : " << std::dec << int(s.data.processor_family_2)
       << "\n\tCoreCount2               : " << std::dec << int(s.data.core_count_2)
       << "\n\tCoreEnabled2             : " << std::dec << int(s.data.core_enabled_2)
       << "\n\tThreadCount2             : " << std::dec << int(s.data.thread_count_2) << std::endl;
    return os;
}


std::ostream& operator<<(std::ostream& os, const StructEnhanced<smbios::parser::SMBIOS_CPUID_DATA>& s) {
    os << "CPUID DATA\t@" << s.header.handle << "\n";
    os << "\tSocket designation   : " << s.get_string(s.data.socket_designation)
       << "\n\tSubType              : 0x" << std::hex << static_cast<uint32_t>(s.data.socket_designation) << std::endl;
    return os;
}


std::ostream& operator<<(std::ostream& os, const StructEnhanced<smbios::parser::SMBIOS_CPUID_DATA_V2>& s) {
    os << "CPUID DATA V2\t@" << s.header.handle << "\n";
    os << "\tSocket designation   : " << s.get_string(s.data.socket_designation)
       << "\n\tSubType              : 0x" << std::hex << static_cast<uint32_t>(s.data.socket_designation) << std::endl;
    return os;
}


std::ostream& operator<<(std::ostream& os, const StructEnhanced<smbios::parser::SMBIOS_STORAGE_INFO_DATA>& s) {
    os << "HDD_INFO_DATA\t@" << s.header.handle << "\n";
    os << "\tPciClassCode    : 0x" << std::hex << static_cast<uint32_t> (s.data.PciClassCode)
       << "\n\tVendorID        : 0x" << std::hex << s.data.VendorID
       << "\n\tDeviceID        : 0x" << std::hex << s.data.DeviceID
       << "\n\tSubVendorID     : 0x" << std::hex << s.data.SubVendorID
       << "\n\tSubDeviceID     : 0x" << std::hex << s.data.SubDeviceID
       << "\n\tHDDIndex        : " << std::dec << static_cast<uint32_t> (s.data.RsaHddInfoData.HDDIndex)
       << "\n\tHDDName         : " << std::string(reinterpret_cast<const char*>(s.data.RsaHddInfoData.HDDName),
                                                  smbios::parser::HDD_INFO_DATA::HDD_NAME_STRING_SIZE)
       << "\n\tHDDType         : 0x" << std::dec << static_cast<uint32_t> (s.data.RsaHddInfoData.HDDType)
       << "\n\tHDDSizeInGB     : " << std::dec << s.data.RsaHddInfoData.HDDSizeInGB
       << std::endl;
    return os;
}


std::ostream& operator<<(std::ostream& os, const StructEnhanced<smbios::parser::SMBIOS_STORAGE_INFO_DATA_V2>& s) {
    os << "HDD_INFO_DATA\t@" << s.header.handle << "\n";
    os << "\tPciClassCode    : 0x" << std::hex << static_cast<uint32_t> (s.data.PciClassCode)
       << "\n\tPciSubClassCode : 0x" << std::hex << static_cast<uint32_t> (s.data.PciSubClassCode)
       << "\n\tVendorID        : 0x" << std::hex << s.data.VendorID
       << "\n\tDeviceID        : 0x" << std::hex << s.data.DeviceID
       << "\n\tSubVendorID     : 0x" << std::hex << s.data.SubVendorID
       << "\n\tSubDeviceID     : 0x" << std::hex << s.data.SubDeviceID
       << "\n\tHDDIndex        : " << std::dec << static_cast<uint32_t> (s.data.RsaHddInfoData.HDDIndex)
       << "\n\tHDDName         : " << std::string(reinterpret_cast<const char*>(s.data.RsaHddInfoData.HDDName),
                                                  smbios::parser::HDD_INFO_DATA_V2::HDD_NAME_STRING_SIZE)
       << "\n\tHDDSerialNumber : " << std::string(reinterpret_cast<const char*>(s.data.RsaHddInfoData.HDDSerialNumber),
                                                  smbios::parser::HDD_INFO_DATA_V2::HDD_SERIAL_NUMBER_SIZE)
       << "\n\tHDDType         : 0x" << std::dec << static_cast<uint32_t> (s.data.RsaHddInfoData.HDDType)
       << "\n\tHDDSizeInGB     : " << std::dec << s.data.RsaHddInfoData.HDDSizeInGB
       << std::endl;
    return os;
}


std::ostream& operator<<(std::ostream& os, const StructEnhanced<smbios::parser::SMBIOS_NIC_INFO_DATA>& s) {
    os << "NIC_INFO_DATA\t@" << s.header.handle << "\n";
    os << "\tPciClassCode    : 0x" << std::hex << static_cast<uint32_t>(s.data.PciClassCode)
       << "\n\tSlotNo          : 0x" << std::hex << s.data.SlotNo
       << "\n\tVendorID        : 0x" << std::hex << s.data.VendorID
       << "\n\tDeviceID        : 0x" << std::hex << s.data.DeviceID
       << "\n\tSubVendorID     : 0x" << std::hex << s.data.SubVendorID
       << "\n\tSubDeviceID     : 0x" << std::hex << s.data.SubDeviceID
       << "\n\tLinkSpeed       : " << std::dec << s.data.LinkSpeed
       << "\n\tLinkWidth       : " << std::dec << s.data.LinkWidth
       << "\n\tPortIndex       : " << std::dec << s.data.PortData.PortIndex
       << "\n\tMAC             : " << std::hex << std::setw(2) << std::setfill('0')
       << static_cast<int>(s.data.PortData.Mac[0]) << ":"
       << static_cast<int>(s.data.PortData.Mac[1]) << ":"
       << static_cast<int>(s.data.PortData.Mac[2]) << ":"
       << static_cast<int>(s.data.PortData.Mac[3]) << ":"
       << static_cast<int>(s.data.PortData.Mac[4]) << ":"
       << static_cast<int>(s.data.PortData.Mac[5]) << std::endl;

    return os;
}


std::ostream& operator<<(std::ostream& os, const StructEnhanced<smbios::parser::SMBIOS_NIC_INFO_DATA_V2>& s) {
    os << "NIC_INFO_DATA\t@" << s.header.handle << "\n";
    os << "\tPciClassCode    : 0x" << std::hex << static_cast<uint32_t>(s.data.PciClassCode)
       << "\n\tSlotNo          : 0x" << std::hex << s.data.SlotNo
       << "\n\tVendorID        : 0x" << std::hex << s.data.VendorID
       << "\n\tDeviceID        : 0x" << std::hex << s.data.DeviceID
       << "\n\tSubVendorID     : 0x" << std::hex << s.data.SubVendorID
       << "\n\tSubDeviceID     : 0x" << std::hex << s.data.SubDeviceID
       << "\n\tLinkSpeed       : " << std::dec << s.data.LinkSpeed
       << "\n\tLinkWidth       : " << std::dec << s.data.LinkWidth
       << "\n\tPortIndex       : " << std::dec << s.data.PortData.PortIndex
       << "\n\tMAC             : " << std::hex << std::setw(2) << std::setfill('0')
       << static_cast<int>(s.data.PortData.Mac[0]) << ":"
       << static_cast<int>(s.data.PortData.Mac[1]) << ":"
       << static_cast<int>(s.data.PortData.Mac[2]) << ":"
       << static_cast<int>(s.data.PortData.Mac[3]) << ":"
       << static_cast<int>(s.data.PortData.Mac[4]) << ":"
       << static_cast<int>(s.data.PortData.Mac[5])
       << "\n\tFirmwareVersion : " << s.get_string(s.data.FirmwareVersion) << std::endl;

    return os;
}


std::ostream& operator<<(std::ostream& os, const StructEnhanced<smbios::parser::SMBIOS_FPGA_DATA>& s) {
    os << "FPGA_DATA\t@" << s.header.handle << "\n";
    os << "\tFPGAIndex                        : " << std::dec << int{s.data.fpga_index}
       << "\n\tFPGAType                         : 0x" << std::hex << static_cast<int>(s.data.fpga_type)
       << "\n\tFPGAStatus                       : 0x" << std::hex << static_cast<int>(s.data.fpga_status)
       << "\n\tSocketIdentifier                 : " << std::dec << int{s.data.socket_identifier}
       << "\n\tFPGAVendor                       : " << s.get_string(s.data.fpga_vendor)
       << "\n\tFPGAFamily                       : " << s.get_string(s.data.fpga_family)
       << "\n\tFPGAModel                        : " << s.get_string(s.data.fpga_model)
       << "\n\tFPGABitStreamVersion             : " << s.get_string(s.data.fpga_bit_stream_version)
       << "\n\tFPGAHPSCoreCount                 : " << std::dec << uint32_t{s.data.fpga_hps_core_count}
       << "\n\tFPGAHPSISA                       : " << std::dec << uint32_t{s.data.fpga_hps_isa}
       << "\n\tFPGAHSSIConfiguration            : 0x" << std::hex << static_cast<int>(s.data.fpga_hssi_configuration)
       << "\n\tFPGIHSSIPortCount                : " << std::dec << uint32_t{s.data.fpga_hssi_port_count}
       << "\n\tFPGIHSSIPortSpeed                : " << std::dec << uint32_t{s.data.fpga_hssi_port_speed}
       << "\n\tFPGAHSSISideBandConfiguration    : " << s.get_string(s.data.fpga_hssi_side_band_configuration)
       << "\n\tFPGAReconfigurationSlots         : " << std::dec << uint32_t{s.data.fpga_reconfiguration_slots}
       << "\n\tPCIeSlotNumber                   : " << std::dec << uint32_t{s.data.fpga_pcie_slot_number}
       << "\n\tPCIeBusIdentifier                : " << std::dec << uint32_t{s.data.fpga_pcie_bus_identifier}
       << "\n\tPCIeDeviceIdentifier             : " << std::dec << uint32_t{s.data.fpga_pcie_device_identifier}
       << "\n\tPCIeFunctionIdentifier           : " << std::dec << uint32_t{s.data.fpga_pcie_function_identifier}
       << "\n\tThermalDesignPower               : " << std::dec << s.data.thermal_design_power
       << "\n\tIntegratedMemoryTechnology        : " << std::dec << static_cast<uint32_t>(s.data.memory_technology)
       << "\n\tIntegratedMemoryCapacity          : " << std::dec << s.data.integrated_memory_capacity
       << "\n\tIntegratedMemorySpeed             : " << std::dec << uint{s.data.integrated_memory_speed} << std::endl;

    return os;
}


std::ostream& operator<<(std::ostream& os, const StructEnhanced<smbios::parser::SMBIOS_FPGA_DATA_OEM>& s) {
    os << "FPGA_DATA\t@" << s.header.handle << "\n";
    os << "\tFPGAIndex                        : " << std::dec << int{s.data.fpga_index}
       << "\n\tFPGAType                         : 0x" << std::hex << static_cast<int>(s.data.fpga_type)
       << "\n\tFPGAStatus                       : 0x" << std::hex << static_cast<int>(s.data.fpga_status)
       << "\n\tSocketIdentifier                 : " << std::dec << int{s.data.socket_identifier}
       << "\n\tFPGAVendor                       : " << s.get_string(s.data.fpga_vendor)
       << "\n\tFPGAFamily                       : " << s.get_string(s.data.fpga_family)
       << "\n\tFPGAModel                        : " << s.get_string(s.data.fpga_model)
       << "\n\tFPGABitStreamVersion             : " << s.get_string(s.data.fpga_bit_stream_version)
       << "\n\tFPGAHPSCoreCount                 : " << std::dec << uint32_t{s.data.fpga_hps_core_count}
       << "\n\tFPGAHPSISA                       : " << std::dec << uint32_t{s.data.fpga_hps_isa}
       << "\n\tFPGAHSSIConfiguration            : 0x" << std::hex << static_cast<int>(s.data.fpga_hssi_configuration)
       << "\n\tFPGIHSSIPortCount                : " << std::dec << uint32_t{s.data.fpga_hssi_port_count}
       << "\n\tFPGIHSSIPortSpeed                : " << std::dec << uint32_t{s.data.fpga_hssi_port_speed}
       << "\n\tFPGAHSSISideBandConfiguration    : " << s.get_string(s.data.fpga_hssi_side_band_configuration)
       << "\n\tFPGAReconfigurationSlots         : " << std::dec << uint32_t{s.data.fpga_reconfiguration_slots}
       << "\n\tPCIeSlotNumber                   : " << std::dec << uint32_t{s.data.fpga_pcie_slot_number}
       << "\n\tPCIeBusIdentifier                : " << std::dec << uint32_t{s.data.fpga_pcie_bus_identifier}
       << "\n\tPCIeDeviceIdentifier             : " << std::dec << uint32_t{s.data.fpga_pcie_device_identifier}
       << "\n\tPCIeFunctionIdentifier           : " << std::dec << uint32_t{s.data.fpga_pcie_function_identifier}
       << "\n\tThermalDesignPower               : " << std::dec << s.data.thermal_design_power
       << "\n\tIntegratedMemoryTechnology        : " << std::dec << static_cast<uint32_t>(s.data.memory_technology)
       << "\n\tIntegratedMemoryCapacity          : " << std::dec << s.data.integrated_memory_capacity
       << "\n\tIntegratedMemorySpeed             : " << std::dec << uint{s.data.integrated_memory_speed} << std::endl;

    return os;
}


std::ostream& operator<<(std::ostream& os, const StructEnhanced<smbios::parser::SMBIOS_STORAGE_DEVICE_INFO_DATA>& s) {
    os << "STORAGE_DEVICE_DATA\t@" << s.header.handle << "\n";
    os << "\tPortDesignation                  : " << std::dec << uint32_t{s.data.port_designation}
       << "\n\tDeviceIndex                      : " << std::dec << uint32_t{s.data.device_index}
       << "\n\tConnectorType                    : 0x" << std::hex << static_cast<uint32_t>(s.data.connector_type)
       << "\n\tDeviceProtocol                   : 0x" << std::hex << static_cast<uint32_t>(s.data.device_protocol)
       << "\n\tDeviceType                       : 0x" << std::hex << static_cast<uint32_t>(s.data.device_type)
       << "\n\tDeviceCapacity                   : " << std::dec << uint32_t{s.data.device_capacity}
       << "\n\tDeviceRPM                        : " << std::dec << std::uint32_t{s.data.device_rpm}
       << "\n\tDeviceModel                      : " << s.get_string(s.data.device_model)
       << "\n\tDeviceSerial                     : " << s.get_string(s.data.device_serial)
       << "\n\tPCIClassCode                     : 0x" << std::hex << uint32_t{s.data.pci_class_code}
       << "\n\tVendorId                         : 0x" << std::hex << uint32_t{s.data.vendor_id}
       << "\n\tDeviceId                         : 0x" << std::hex << uint32_t{s.data.device_id}
       << "\n\tSubVendorId                      : 0x" << std::hex << uint32_t{s.data.sub_vendor_id}
       << "\n\tSubDeviceId                      : 0x" << std::hex << uint32_t{s.data.sub_device_id}
       << "\n\tFirmwareVersion                  : " << s.get_string(s.data.firmware_version) << std::endl;

    return os;
}


std::ostream& operator<<(std::ostream& os, const StructEnhanced<smbios::parser::SMBIOS_TPM_INFO_DATA>& s) {
    os << "TPM_DATA\t@" << s.header.handle << "\n";
    os << "\tConfigurationIndex       : " << std::dec << uint32_t{s.data.tpm_configuration_index}
       << "\n\tVersion                  : " << s.get_string(s.data.tpm_version)
       << "\n\tStatus                   : 0x" << std::hex << static_cast<uint32_t>(s.data.tpm_status) << std::endl;

    return os;
}


std::ostream& operator<<(std::ostream& os, const StructEnhanced<smbios::parser::SMBIOS_TXT_INFO_DATA>& s) {
    os << "TXT DEVICE DATA\t@" << s.header.handle << "\n";
    os << "\tTxtStatus      : 0x" << std::hex << static_cast<uint32_t>(s.data.status) << std::endl;
    return os;
}


std::ostream&
operator<<(std::ostream& os, const StructEnhanced<smbios::parser::SMBIOS_MEMORY_DEVICE_EXTENDED_INFO_DATA>& s) {
    os << "MEMORY DEVICE EXTENDED DATA\t@" << s.header.handle << "\n";
    os << "\tMemoryDeviceHandle        : " << std::dec << uint32_t{s.data.memory_device_handle}
       << "\n\tMemoryType                : 0x" << std::hex << static_cast<uint32_t>(s.data.type)
       << "\n\tMemoryMedia               : 0x" << std::hex << static_cast<uint32_t>(s.data.media)
       << "\n\tMemoryFirmwareRevision    : " << s.get_string(s.data.firmware_revision)
       << "\n\tMemoryFirmwareAPIVersion  : " << s.get_string(s.data.firmware_api_version)
       << "\n\tMaxTDP                    : " << std::dec << uint32_t{s.data.max_tdp}
       << "\n\tMemorySMBusAddress        : " << std::dec << static_cast<uint32_t>(s.data.smbus_address) << std::endl;
    return os;
}


std::ostream& operator<<(std::ostream& os, const StructEnhanced<smbios::parser::SMBIOS_MEMORY_DEVICE>& s) {
    os << "MEMORY DEVICE DATA\t@" << s.header.handle << "\n";
    os << "\tPhysicalMemoryArrayHandle         : 0x" << std::hex << s.data.physical_memory_array_handle
       << "\n\tMemoryErrorInfoHandle             : 0x" << std::hex
       << static_cast<uint32_t>(s.data.memory_error_info_handle)
       << "\n\tTotal_width                       : " << std::dec << static_cast<uint32_t>(s.data.total_width)
       << "\n\tData_width                        : " << std::dec << static_cast<uint32_t>(s.data.data_width)
       << "\n\tSize                              : " << std::dec << static_cast<uint32_t>(s.data.size)
       << "\n\tFormFactorEnum                    : 0x" << std::hex << static_cast<uint32_t>(s.data.form_factor)
       << "\n\tDevice_set                        : 0x" << std::hex << static_cast<uint32_t>(s.data.device_set)
       << "\n\tDevice_locator                    : " << s.get_string(s.data.device_locator)
       << "\n\tBank_locator                      : " << s.get_string(s.data.bank_locator)
       << "\n\tMemory_type                       : 0x" << std::hex << static_cast<uint32_t>(s.data.memory_type)
       << "\n\tType_detail                       : " << std::bitset<16>{s.data.type_detail}.to_string()
       << "\n\tSpeed                             : " << std::dec << uint32_t{s.data.speed}
       << "\n\tManufacturer                      : " << s.get_string(s.data.manufacturer)
       << "\n\tSerial_number                     : " << s.get_string(s.data.serial_number)
       << "\n\tAsset_tag                         : " << s.get_string(s.data.asset_tag)
       << "\n\tPart_number                       : " << s.get_string(s.data.part_number)
       << "\n\tAttributes                        : 0x" << std::hex << uint32_t{s.data.attributes}
       << "\n\tExtended_size                     : " << std::dec << uint32_t{s.data.extended_size}
       << "\n\tConfigured_memory_clock_speed     : " << std::dec << uint32_t{s.data.configured_memory_clock_speed}
       << "\n\tVoltage_min                       : " << std::dec << uint32_t{s.data.voltage_min}
       << "\n\tVoltage_max                       : " << std::dec << uint32_t{s.data.voltage_max}
       << "\n\tVoltage_configured                : " << std::dec << uint32_t{s.data.voltage_configured}
       << std::endl;
    return os;
}


std::ostream& operator<<(std::ostream& os, const StructEnhanced<smbios::parser::SMBIOS_PCIE_PORT_INFO_DATA>& s) {
    os << "CABLED PCIE PORT DATA\t@" << s.header.handle << "\n";
    return os;
}


std::ostream& operator<<(std::ostream& os, const StructEnhanced<smbios::parser::SMBIOS_SPEED_SELECT_INFO_DATA>& s) {
    os << "SPEED SELECT DATA\t@" << s.header.handle << "\n";
    os << "\tSocket_number               : " << std::dec << uint32_t(s.data.socket_number)
       << "\n\tStructure_version           : " << uint32_t(s.data.structure_version)
       << "\n\tNumber_of_configs           : " << uint32_t(s.data.number_of_configs)
       << "\n\tCurrent_config              : " << uint32_t(s.data.current_config);

    for (const auto& config : s.configs) {
       os << "\n\n\tConfiguration_number        : " << uint32_t(config.configuration_number)
          << "\n\tHigh_priority_core_count    : " << uint32_t(config.high_priority_core_count)
          << "\n\tHigh_priority_base_frequency: " << uint32_t(config.high_priority_base_frequency)
          << "\n\tLow_priority_core_count     : " << uint32_t(config.low_priority_core_count)
          << "\n\tLow_priority_base_frequency : " << uint32_t(config.low_priority_base_frequency)
          << "\n\tMax_tdp                     : " << config.max_tdp
          << "\n\tMax_junction_temperature    : " << uint32_t(config.max_junction_temperature)
          << "\n\tHigh_priority_code_apic_ids" << s.get_string(config.high_priority_code_apic_ids);
    }

    return os << std::endl;
}

}
