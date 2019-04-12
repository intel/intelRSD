/*!
 * @brief ACPI tables parser implementation
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
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
 * @file acpi_parser.cpp
 */

#include "acpi/acpi_parser.hpp"
#include "utils/conversion.hpp"
#include <ostream>
#include <iomanip>
#include <bitset>

namespace acpi {
namespace parser {

template<typename T>
using StructEnhanced = AcpiParser::StructEnhanced<T>;


AcpiParser::Exception::~Exception() {}

}
}

namespace mdr {

template<>
std::shared_ptr<const uint8_t> acpi::parser::AcpiParser::init_buffer(const uint8_t* _buf, size_t _buf_size) {
    if (nullptr == _buf) {
        throw Exception("Invalid ACPI blob pointer");
    }
    auto buffer = std::shared_ptr<const uint8_t>(new uint8_t[_buf_size], std::default_delete<uint8_t[]>());
    std::copy(_buf, _buf + _buf_size, const_cast<uint8_t*>(buffer.get()));
    return buffer;
}

template<>
std::ostream& operator<<(std::ostream& os, const acpi::parser::AcpiParser& parser) {
    uint64_t table_offset = 0x00;
    while (table_offset < parser.buf_size) {
        const acpi::structs::ACPI_TABLE_HEADER& acpi_header =
            *reinterpret_cast<const acpi::structs::ACPI_TABLE_HEADER*>(parser.buf.get() + table_offset);

        std::string acpi_signature = {reinterpret_cast<char const*>(acpi_header.Signature), sizeof(acpi_header.Signature)};
        uint64_t subtable_offset = table_offset + sizeof(acpi::structs::ACPI_TABLE_HEADER);
        uint64_t table_end_offset = table_offset + acpi_header.Length;

        if (acpi_signature == acpi::structs::NFIT) {
            while (subtable_offset + sizeof(acpi::structs::NFIT_SUBTABLE_HEADER) < table_end_offset) {
                const acpi::structs::NFIT_SUBTABLE_HEADER& subtable_header =
                    *reinterpret_cast<const acpi::structs::NFIT_SUBTABLE_HEADER*>(parser.buf.get() + subtable_offset);

                if (subtable_offset + subtable_header.length > table_end_offset) {
                    throw acpi::parser::AcpiParser::Exception("Unexpectedly reached end of ACPI NFIT table");
                }

                switch (subtable_header.type) {
                    case acpi::structs::NFIT_SPA_RANGE_STRUCTURE::ID: {
                        const auto data = parser.read_struct<acpi::structs::NFIT_SPA_RANGE_STRUCTURE>(subtable_offset);
                        os << data;
                        break;
                    }
                    case acpi::structs::NFIT_NVDIMM_REGION_MAPPING_STRUCTURE::ID: {
                        const auto data = parser.read_struct<acpi::structs::NFIT_NVDIMM_REGION_MAPPING_STRUCTURE>(subtable_offset);
                        os << data;
                        break;
                    }
                    case acpi::structs::NFIT_NVDIMM_CONTROL_REGION_STRUCTURE::ID: {
                        const auto data = parser.read_struct<acpi::structs::NFIT_NVDIMM_CONTROL_REGION_STRUCTURE>(subtable_offset);
                        os << data;
                        break;
                    }
                    case acpi::structs::NFIT_NVDIMM_BLOCK_DATA_WINDOW_REGION_STRUCTURE::ID: {
                        const auto data = parser.read_struct<acpi::structs::NFIT_NVDIMM_BLOCK_DATA_WINDOW_REGION_STRUCTURE>(subtable_offset);
                        os << data;
                        break;
                    }
                    case acpi::structs::NFIT_PLATFORM_CAPABILITIES_STRUCTURE::ID: {
                        const auto data = parser.read_struct<acpi::structs::NFIT_PLATFORM_CAPABILITIES_STRUCTURE>(subtable_offset);
                        os << data;
                        break;
                    }
                    case acpi::structs::NfitSubtableType::UNSUPPORTED:
                    default:
                        os << "Unsupported ACPI NFIT Subtable "
                           << "[Type = " << std::dec << int(subtable_header.type)
                           << " Length = " << std::dec << int(subtable_header.length) << "]" << std::endl;
                        subtable_offset += subtable_header.length;
                        break;
                }
            }
        }
        else if (acpi_signature == acpi::structs::PCAT) {
            while (subtable_offset + sizeof(acpi::structs::PCAT_SUBTABLE_HEADER) < table_end_offset) {
                const acpi::structs::PCAT_SUBTABLE_HEADER& subtable_header =
                    *reinterpret_cast<const acpi::structs::PCAT_SUBTABLE_HEADER*>(parser.buf.get() + subtable_offset);

                if (subtable_offset + subtable_header.length > table_end_offset) {
                    throw acpi::parser::AcpiParser::Exception("Unexpectedly reached end of ACPI PCAT table");
                }

                switch (subtable_header.type) {
                    case acpi::structs::PCAT_PLATFORM_CAPABILITY_INFORMATION_STRUCTURE::ID: {
                        const auto data = parser.read_struct<acpi::structs::PCAT_PLATFORM_CAPABILITY_INFORMATION_STRUCTURE>(subtable_offset);
                        os << data;
                        break;
                    }
                    case acpi::structs::PCAT_SOCKET_SKU_INFORMATION_STRUCTURE::ID: {
                        const auto data = parser.read_struct<acpi::structs::PCAT_SOCKET_SKU_INFORMATION_STRUCTURE>(subtable_offset);
                        os << data;
                        break;
                    }
                    case acpi::structs::PcatSubtableType::UNSUPPORTED:
                    default:
                        os << "Unsupported ACPI PCAT Subtable "
                           << "[Type = " << std::dec << int(subtable_header.type)
                           << " Length = " << std::dec << int(subtable_header.length) << "]" << std::endl;
                        subtable_offset += subtable_header.length;
                        break;
                }
            }
        }
        else {
            os << "Unsupported ACPI Table "
               << "[Signature = " << acpi_signature << " Length = " << std::dec << int(acpi_header.Length) << "]" << std::endl;
        }

        table_offset += acpi_header.Length;
    }

    return os;
}

std::ostream& operator<<(std::ostream& os, const StructEnhanced<acpi::structs::NFIT_SPA_RANGE_STRUCTURE>& s) {
    os << "NFIT SPA Range Structure "
       << "[Type = " << std::dec << int(s.header.type) << " Length = " << std::dec << int(s.header.length) << "]";
    os << "\n\tSPA Range Structure Index : " << std::dec << int(s.data.SpaRangeStructureIndex)
       << "\n\tFlags : " << std::bitset<16>(s.data.Flags).to_string()
       << "\n\tProximity Domain : " << std::hex << std::setfill('0') << std::setw(8) << s.data.ProximityDomain
       << "\n\tAddress Range Type GUID : " << utils::buffer_to_string(s.data.AddressRangeTypeGuid, sizeof(s.data.AddressRangeTypeGuid))
       << "\n\tSystem Physical Address Range Base : " << std::hex << std::setfill('0') << std::setw(16) << s.data.SystemPhysicalAddressRangeBase
       << "\n\tSystem Physical Address Range Length : " << std::hex << std::setfill('0') << std::setw(16) << s.data.SystemPhysicalAddressRangeLength
       << "\n\tAddress Range Memory Mapping Attribute : " << std::hex << std::setfill('0') << std::setw(16) << s.data.AddressRangeMemoryMappingAttribute
       << std::endl;
    return os;
}

std::ostream& operator<<(std::ostream& os, const StructEnhanced<acpi::structs::NFIT_NVDIMM_REGION_MAPPING_STRUCTURE>& s) {
    os << "NFIT NVDIMM Region Mapping Structure "
       << "[Type = " << std::dec << int(s.header.type) << " Length = " << std::dec << int(s.header.length) << "]";
    os << "\n\tNFIT Device Handle : " << std::hex << std::setfill('0') << std::setw(8) << s.data.NfitDeviceHandle
       << "\n\tNVDIMM Physical ID : " << std::dec << int(s.data.NvdimmPhysicalID)
       << "\n\tNVDIMM Region ID : " << std::dec << int(s.data.NvdimmRegionID)
       << "\n\tSPA Range Structure Index : " << std::dec << int(s.data.SpaRangeStructureIndex)
       << "\n\tNVDIMM Control Region Structure Index : " << std::dec << int(s.data.NvdimmControlRegionStructureIndex)
       << "\n\tNVDIMM Region Size : " << std::hex << std::setfill('0') << std::setw(16) << s.data.NvdimmRegionSize
       << "\n\tRegion Offset: " << std::hex << std::setfill('0') << std::setw(16) << s.data.RegionOffset
       << "\n\tNVDIMM Physical Address Region Base : " << std::hex << std::setfill('0') << std::setw(16) << s.data.NvdimmPhysicalAddressRegionBase
       << "\n\tInterleave Structure Index : " << std::dec << int(s.data.InterleaveStructureIndex)
       << "\n\tInterleave Ways : " << std::hex << std::setfill('0') << std::setw(4) << s.data.InterleaveWays
       << "\n\tNVDIMM State Flags : " << std::bitset<16>(s.data.NvdimmStateFlags).to_string()
       << std::endl;
    return os;
}

std::ostream& operator<<(std::ostream& os, const StructEnhanced<acpi::structs::NFIT_NVDIMM_CONTROL_REGION_STRUCTURE>& s) {
    os << "NFIT NVDIMM Control Region Structure "
       << "[Type = " << std::dec << int(s.header.type) << " Length = " << std::dec << int(s.header.length) << "]";
    os << "\n\tNVDIMM Control Region Structure Index : " << std::dec << int(s.data.NvdimmControlRegionStructureIndex)
       << "\n\tVendor ID : " << std::hex << std::setfill('0') << std::setw(4) << s.data.VendorID
       << "\n\tDevice ID : " << std::hex << std::setfill('0') << std::setw(4) << s.data.DeviceID
       << "\n\tRevision ID : " << std::hex << std::setfill('0') << std::setw(4) << s.data.RevisionID
       << "\n\tSubsystem Vendor ID : " << std::hex << std::setfill('0') << std::setw(4) << s.data.SubsystemVendorID
       << "\n\tSubsystem Device ID : " << std::hex << std::setfill('0') << std::setw(4) << s.data.SubsystemDeviceID
       << "\n\tSubsystem Revision ID : " << std::hex << std::setfill('0') << std::setw(4) << s.data.SubsystemRevisionID
       << "\n\tValid Fields : " << std::bitset<8>(s.data.ValidFields).to_string()
       << "\n\tManufacturing Location : " << std::hex << std::setfill('0') << std::setw(2) << s.data.ManufacturingLocation
       << "\n\tManufacturing Date : " << std::hex << std::setfill('0') << std::setw(4) << s.data.ManufacturingDate
       << "\n\tSerial Number : " << std::hex << std::setfill('0') << std::setw(4) << s.data.SerialNumber
       << "\n\tRegion Format Interface Code : " << std::hex << std::setfill('0') << std::setw(4) << s.data.RegionFormatInterfaceCode
       << "\n\tNumber Of Block Control Windows : " << std::dec << int(s.data.NumberOfBlockControlWindows)
       // Below applies for 80B structure version only - not used currently
       /*
       << "\n\tSize Of Block Control Windows : " << std::hex << std::setfill('0') << std::setw(16) << s.data.SizeOfBlockControlWindows
       << "\n\tCommand Register Offset In Block Control Window : " << std::hex << std::setfill('0') << std::setw(16) << s.data.CommandRegisterOffsetInBlockControlWindow
       << "\n\tSize Of Command Register In Block Control Windows : " << std::hex << std::setfill('0') << std::setw(16) << s.data.SizeOfCommandRegisterInBlockControlWindows
       << "\n\tStatus Register Offset In Block Control Window : " << std::hex << std::setfill('0') << std::setw(16) << s.data.StatusRegisterOffsetInBlockControlWindow
       << "\n\tSize Of Status Register In Block Control Windows : " << std::hex << std::setfill('0') << std::setw(16) << s.data.SizeOfStatusRegisterInBlockControlWindows
       << "\n\tNVDIMM Control Region Flag : " << std::bitset<16>(s.data.NvdimmControlRegionFlag).to_string()
       */
       << std::endl;
    return os;
}

std::ostream& operator<<(std::ostream& os, const StructEnhanced<acpi::structs::NFIT_NVDIMM_BLOCK_DATA_WINDOW_REGION_STRUCTURE>& s) {
    os << "NFIT NVDIMM Block Data Window Region Structure "
       << "[Type = " << std::dec << int(s.header.type) << " Length = " << std::dec << int(s.header.length) << "]";
    os << "\n\tNVDIMM Control Region Structure Index : " << std::dec << int(s.data.NvdimmControlRegionStructureIndex)
       << "\n\tNumber Of Block Data Windows : " << std::dec << int(s.data.NumberOfBlockDataWindows)
       << "\n\tBlock Data Window Start Offset : " << std::hex << std::setfill('0') << std::setw(16) << s.data.BlockDataWindowStartOffset
       << "\n\tSize Of Block Data Window : " << std::hex << std::setfill('0') << std::setw(16) << s.data.SizeOfBlockDataWindow
       << "\n\tBlock Accessible Memory Capacity : " << std::hex << std::setfill('0') << std::setw(16) << s.data.BlockAccessibleMemoryCapacity
       << "\n\tBeginning Address Of First Block In Block Accessible Memory : " << std::hex << std::setfill('0') << std::setw(16) << s.data.BeginningAddressOfFirstBlockInBlockAccessibleMemory
       << std::endl;
    return os;
}

std::ostream& operator<<(std::ostream& os, const StructEnhanced<acpi::structs::NFIT_PLATFORM_CAPABILITIES_STRUCTURE>& s) {
    os << "NFIT Platform Capabilities Structure "
       << "[Type = " << std::dec << int(s.header.type) << " Length = " << std::dec << int(s.header.length) << "]";
    os << "\n\tHighest Valid Capability : " << std::dec << int(s.data.HighestValidCapability)
       << "\n\tCapabilities : " << std::bitset<32>(s.data.Capabilities).to_string()
       << std::endl;
    return os;
}

std::ostream& operator<<(std::ostream& os, const StructEnhanced<acpi::structs::PCAT_PLATFORM_CAPABILITY_INFORMATION_STRUCTURE>& s) {
    os << "PCAT Platform Capability Information Structure "
       << "[Type = " << std::dec << int(s.header.type) << " Length = " << std::dec << int(s.header.length) << "]";
    os << "\n\tIntel NVDIMM Management SW Config Input Support : " << std::bitset<8>(s.data.IntelNvdimmManagementSwConfigInputSupport).to_string()
       << "\n\tMemory Mode Capabilities : " << std::bitset<8>(s.data.MemoryModeCapabilities).to_string()
       << "\n\tCurrent Memory Mode : " << std::bitset<8>(s.data.CurrentMemoryMode).to_string()
       << "\n\tPersistent Memory RAS Capability : " << std::bitset<8>(s.data.PersistentMemoryRasCapability).to_string()
       << std::endl;
    return os;
}

std::ostream& operator<<(std::ostream& os, const StructEnhanced<acpi::structs::PCAT_SOCKET_SKU_INFORMATION_STRUCTURE>& s) {
    os << "PCAT Socket SKU Information Structure "
       << "[Type = " << std::dec << int(s.header.type) << " Length = " << std::dec << int(s.header.length) << "]";
    os << "\n\tSocket ID : " << std::dec << int(s.data.SocketID)
       << "\n\tMapped Memory Size Limit : " << std::dec << int(s.data.MappedMemorySizeLimit)
       << "\n\tTotal Memory Size Mapped Into SPA : " << std::dec << int(s.data.TotalMemorySizeMappedIntoSpa)
       << "\n\tMemory Size Excluded In Limit Calculations When In 2LM Mode : " << std::dec << int(s.data.MemorySizeExcludedInLimitCalculationsWhenIn2LmMode)
       << std::endl;
    return os;
}

}