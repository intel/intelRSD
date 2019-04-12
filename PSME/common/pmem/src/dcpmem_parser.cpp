/*!
 * @brief Intel Optane DC Persistent Memory FW Commands parser implementation
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
 * @file dcpmem_parser.cpp
 */

#include "pmem/dcpmem_parser.hpp"
#include "utils/conversion.hpp"
#include <ostream>
#include <iomanip>
#include <bitset>

namespace pmem {
namespace parser {

template<typename T>
using StructEnhanced = DcpmemParser::StructEnhanced<T>;


DcpmemParser::Exception::~Exception() {}

}
}

namespace mdr {

template<>
std::shared_ptr<const uint8_t> pmem::parser::DcpmemParser::init_buffer(const uint8_t* _buf, size_t _buf_size) {
    if (nullptr == _buf) {
        throw Exception("Invalid DCPMEM blob pointer");
    }
    auto buffer = std::shared_ptr<const uint8_t>(new uint8_t[_buf_size], std::default_delete<uint8_t[]>());
    std::copy(_buf, _buf + _buf_size, const_cast<uint8_t*>(buffer.get()));
    return buffer;
}


template<>
std::ostream& operator<<(std::ostream& os, const pmem::parser::DcpmemParser& parser) {
    uint64_t offset = 0;
    while (offset + sizeof(pmem::structs::DCPMEM_RESPONSE_HEADER) < parser.buf_size) {
        const pmem::structs::DCPMEM_RESPONSE_HEADER& header =
            *reinterpret_cast<const pmem::structs::DCPMEM_RESPONSE_HEADER*>(parser.buf.get() + offset);

        if (offset + header.length > parser.buf_size) {
            throw pmem::parser::DcpmemParser::Exception("Unexpectedly reached end of DCPMEM command blob");
        }

        switch (header.type) {
            case pmem::structs::DCPMEM_IDENTIFY_DIMM_DATA::ID: {
                const auto data = parser.read_struct<pmem::structs::DCPMEM_IDENTIFY_DIMM_DATA>(offset);
                os << data;
                break;
            }
            case pmem::structs::DCPMEM_GET_SECURITY_STATE_DATA::ID: {
                const auto data = parser.read_struct<pmem::structs::DCPMEM_GET_SECURITY_STATE_DATA>(offset);
                os << data;
                break;
            }
            case pmem::structs::DCPMEM_GET_POWER_MANAGEMENT_POLICY_DATA::ID: {
                const auto data = parser.read_struct<pmem::structs::DCPMEM_GET_POWER_MANAGEMENT_POLICY_DATA>(offset);
                os << data;
                break;
            }
            case pmem::structs::DCPMEM_GET_DIMM_PARTITION_INFO_DATA::ID: {
                const auto data = parser.read_struct<pmem::structs::DCPMEM_GET_DIMM_PARTITION_INFO_DATA>(offset);
                os << data;
                break;
            }
            case pmem::structs::DCPMEM_GET_SMART_INFO_DATA::ID: {
                const auto data = parser.read_struct<pmem::structs::DCPMEM_GET_SMART_INFO_DATA>(offset);
                os << data;
                break;
            }
            case pmem::structs::DCPMEM_GET_MEMORY_INFO_DATA::ID: {
                const auto data = parser.read_struct<pmem::structs::DCPMEM_GET_MEMORY_INFO_DATA>(offset);
                os << data;
                break;
            }
            case pmem::structs::DcpmemCommandId::UNKNOWN:
            default:
                break;
        }
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const StructEnhanced<pmem::structs::DCPMEM_IDENTIFY_DIMM_DATA>& s) {
    os << "IDENTIFY_DIMM "
       << "[Type = " << std::dec << int(s.header.type)
       << " Length = " << std::dec << int(s.header.length) << "]";
    os << "\n\tVendor ID : " << std::hex << std::setfill('0') << std::setw(4) << s.data.VendorID
       << "\n\tDevice ID : " << std::hex << std::setfill('0') << std::setw(4) << s.data.DeviceID
       << "\n\tRevision ID : " << std::hex << std::setfill('0') << std::setw(4) << s.data.RevisionID
       << "\n\tInterface Format Code : " << std::hex << std::setfill('0') << std::setw(4) << s.data.InterfaceFormatCode
       << "\n\tFirmware Revision : " // Format: aa.bb.cc.dddd
              << std::hex << int(s.data.FirmwareRevision[4]) // aa = Product Number
       << "." << std::hex << int(s.data.FirmwareRevision[3]) // bb = Revision Number
       << "." << std::hex << int(s.data.FirmwareRevision[2]) // cc = Security Revision Number
       << "." << std::hex << int(s.data.FirmwareRevision[0]) << std::hex << int(s.data.FirmwareRevision[1]) // dddd = Build Number
       << "\n\tFeature SW Required Mask : " << std::hex << std::setfill('0') << std::setw(2) << int(s.data.FeatureSwRequiredMask)
       << "\n\tRaw Capacity (in 4KiB units) : " << std::dec << s.data.RawCapacity
       << "\n\tManufacturer : " << std::hex << std::setfill('0') << std::setw(4) << s.data.Manufacturer
       << "\n\tSerial Number : " << std::hex << std::setfill('0') << std::setw(8) << s.data.SerialNumber
       << "\n\tPart Number : " << utils::buffer_to_string(s.data.PartNumber, sizeof(s.data.PartNumber))
       << "\n\tDIMM SKU : " << std::bitset<32>(s.data.DimmSku).to_string()
       << "\n\tAPI Version : " << std::dec << ((s.data.ApiVersion & 0xFF00) >> 8) << "." << (s.data.ApiVersion & 0x00FF)
       << "\n\tDIMM Unique ID : " << utils::buffer_to_string(s.data.DimmUniqueID, sizeof(s.data.DimmUniqueID))
       << std::endl;
    return os;
}

std::ostream& operator<<(std::ostream& os, const StructEnhanced<pmem::structs::DCPMEM_GET_SECURITY_STATE_DATA>& s) {
    os << "SECURITY_STATE "
       << "[Type = " << std::dec << int(s.header.type)
       << " Length = " << std::dec << int(s.header.length) << "]";
    os << "\n\tSecurity Status : " << std::bitset<8>(s.data.SecurityStatus).to_string()
       << std::endl;
    return os;
}

std::ostream& operator<<(std::ostream& os, const StructEnhanced<pmem::structs::DCPMEM_GET_POWER_MANAGEMENT_POLICY_DATA>& s) {
    os << "POWER_MANAGEMENT_POLICY "
       << "[Type = " << std::dec << int(s.header.type)
       << " Length = " << std::dec << int(s.header.length) << "]";
    os << "\n\tPeak Power Budget : " << std::dec << s.data.PeakPowerBudget
       << "\n\tAverage Power Budget : " << std::dec << s.data.AveragePowerBudget
       << std::endl;
    return os;
}

std::ostream& operator<<(std::ostream& os, const StructEnhanced<pmem::structs::DCPMEM_GET_DIMM_PARTITION_INFO_DATA>& s) {
    os << "DIMM_PARTITION_INFO "
       << "[Type = " << std::dec << int(s.header.type)
       << " Length = " << std::dec << int(s.header.length) << "]";
    os << "\n\tVolatile (2LM) Capacity (in 4KiB units) : " << std::dec << s.data.VolatileCapacity
       << "\n\tVolatile Start : 0x" << std::hex << std::setfill('0') << std::setw(16) << s.data.VolatileStart
       << "\n\tPersistent Capacity (in 4KiB units) : " << std::dec << s.data.PersistentCapacity
       << "\n\tPersistent Start : 0x" << std::hex << std::setfill('0') << std::setw(16) << s.data.PersistentStart
       << "\n\tRaw Capacity (in 4KiB units) : " << std::dec << s.data.RawCapacity
       << std::endl;
    return os;
}

std::ostream& operator<<(std::ostream& os, const StructEnhanced<pmem::structs::DCPMEM_GET_SMART_INFO_DATA>& s) {
    os << "SMART_AND_HEALTH_INFO "
       << "[Type = " << std::dec << int(s.header.type)
       << " Length = " << std::dec << int(s.header.length) << "]";
    os << "\n\tValidation Flags : " << std::bitset<32>(s.data.ValidationFlags).to_string()
       << "\n\tHealth Status : " << std::bitset<8>(s.data.HealthStatus).to_string()
       << "\n\tPercentage Remaining : " << std::dec << int(s.data.PercentageRemaining)
       << "\n\tPercentage Used [obsolete] : " << std::dec << int(s.data.PercentageUsed)
       << "\n\tAlarm Trips : " << std::bitset<8>(s.data.AlarmTrips).to_string()
       << "\n\tMedia Temperature (with 0.0625C resolution) : " << std::dec << s.data.MediaTemperature
       << "\n\tController Temperature (with 0.0625C resolution) : " << std::dec << s.data.ControllerTemperature
       << "\n\tDirty Shutdown Count : " << std::dec << s.data.DirtyShutdownCount
       << "\n\tAIT DRAM Status : " << std::hex << std::setfill('0') << std::setw(2) << int(s.data.AitDramStatus)
       << "\n\tHealth Status Reason : " << std::bitset<16>(s.data.HealthStatusReason).to_string()
       << "\n\tLast Shutdown Status : " << std::hex << std::setfill('0') << std::setw(2) << s.data.LastShutdownStatus
       << "\n\tVendor Specific Data Size : " << std::dec << s.data.VendorSpecificDataSize
       << "\n\tVendor Specific Data: "
       << "\n\t\tPower Cycles : " << std::dec << s.data.PowerCycles
       << "\n\t\tPower On Time : " << std::dec << s.data.PowerOnTime
       << "\n\t\tUptime : " << std::dec << s.data.Uptime
       << "\n\t\tUnlatched Dirty Shutdowns : " << std::dec << s.data.UnlatchedDirtyShutdowns
       << "\n\t\tLast Shutdown Status Details : " << std::bitset<8>(s.data.LastShutdownStatusDetails).to_string()
       << "\n\t\tLast Shutdown Time : " << std::dec << s.data.LastShutdownTime
       << "\n\t\tLast Shutdown Status Extended Details : "
       << utils::buffer_to_string(s.data.LastShutdownStatusExtendedDetails, sizeof(s.data.LastShutdownStatusExtendedDetails))
       << std::endl;
    return os;
}

std::ostream& operator<<(std::ostream& os, const StructEnhanced<pmem::structs::DCPMEM_GET_MEMORY_INFO_DATA>& s) {
    os << "MEMORY_INFO "
       << "[Type = " << std::dec << int(s.header.type)
       << " Length = " << std::dec << int(s.header.length) << "]";
    os << "\n\tMedia Reads : " << utils::buffer_to_string(s.data.MediaReads, sizeof(s.data.MediaReads))
       << "\n\tMedia Writes : " << utils::buffer_to_string(s.data.MediaWrites, sizeof(s.data.MediaWrites))
       << "\n\tRead Requests : " << utils::buffer_to_string(s.data.ReadRequests, sizeof(s.data.ReadRequests))
       << "\n\tWrite Requests : " << utils::buffer_to_string(s.data.WriteRequests, sizeof(s.data.WriteRequests))
       << std::endl;
    return os;
}

}
