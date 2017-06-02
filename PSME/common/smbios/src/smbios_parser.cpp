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
 *
 * @file signal.hpp
 *
 * @brief Signal interface
 * */

#include "smbios/smbios_parser.hpp"
#include <numeric>
#include <cstddef>
#include <cstring>
#include <ostream>
#include <iostream>
#include <iomanip>
#include <iterator>
#include <safe-string/safe_lib.hpp>

namespace smbios {
namespace parser {

template<typename T>
using StructEnhanced = SmbiosParser::StructEnhanced<T>;

SmbiosParser::Exception::~Exception() { }

std::shared_ptr<const uint8_t> SmbiosParser::init_buffer(const uint8_t* _buf, size_t _buf_size) {
    if (nullptr == _buf) {
        throw Exception("Invalid SMBIOS blob pointer");
    }
    auto buffer = std::shared_ptr<const uint8_t>(new uint8_t[_buf_size], std::default_delete<uint8_t[]>());
    std::copy(_buf, _buf + _buf_size, const_cast<uint8_t*>(buffer.get()));
    return buffer;
}

const SmbiosEntryPoint &SmbiosParser::read_entry_point() const {
    if (nullptr == buf) {
        throw Exception("Invalid SMBIOS blob pointer");
    }
    if (buf_size < sizeof(SmbiosEntryPoint)) {
        throw Exception("SMBIOS blob too small");
    }
    // verify checksum

    uint8_t sum = std::accumulate(buf.get(), buf.get() + sizeof(SmbiosEntryPoint), uint8_t(0));
    if (0 != sum) {
        throw Exception("Incorrect checksum");
    }
    const SmbiosEntryPoint *ep = reinterpret_cast<const SmbiosEntryPoint *>(buf.get());
    // verify anchor strings
    const uint8_t _SM_[] = {0x5f, 0x53, 0x4D, 0x5F};
    int cmp;
    errno_t res;
    res = ::memcmp_s(ep->anchor, sizeof(ep->anchor), _SM_, sizeof(_SM_), &cmp);
    if (EOK != res || cmp != 0) {
        throw Exception("Invalid anchor string");
    }
    // verify intermediate anchor strings
    const uint8_t _DMI_[] = {0x5f, 0x44, 0x4D, 0x49, 0x5F};
    res = ::memcmp_s(ep->intermediate_anchor, sizeof(ep->intermediate_anchor), _DMI_, sizeof(_DMI_), &cmp);
    if (EOK != res || cmp != 0) {
        throw Exception("Invalid intermediate anchor string");
    }
    // verify intermediate checksum
    sum = std::accumulate(buf.get() + 0x10 /* start of intermediate region */,
                          buf.get() + 0x10 + 0x0F /* end of intermediate region */, uint8_t(0));
    if (0 != sum) {
        throw Exception("Incorrect intermediate checksum");
    }

    return *ep;
}

std::ostream &operator<<(std::ostream &os, const SmbiosParser &parser) {
    os
    << "SMBIOS:\n"
    << "\tStructure Table Length: " << parser.entry_point.struct_table_length << std::endl
    << "\tStructure Table Address: 0x" << std::hex << parser.entry_point.struct_table_address << std::dec << std::endl
    << "\t#Structures: " << parser.entry_point.struct_count << std::endl;

    uint32_t offset = parser.entry_point.struct_table_address;
    while (offset + sizeof(SMBIOS_OOB_HEADER) < parser.buf_size) {
        os << "offset : " << std::dec << offset << std::endl;
        const SMBIOS_OOB_HEADER &header = *reinterpret_cast<const SMBIOS_OOB_HEADER *>(parser.buf.get() + offset);
        if (offset + header.length > parser.buf_size) {
            throw SmbiosParser::Exception("Unexpectedly reached end of smbios blob");
        }
        switch (header.type) {
            case SMBIOS_SYSTEM_INFO_DATA::ID: {
                const auto data = parser.read_struct<SMBIOS_SYSTEM_INFO_DATA>(offset);
                os << data;
                break;
            }
            case SMBIOS_NIC_INFO_DATA::ID: {
                const auto data = parser.read_struct<SMBIOS_NIC_INFO_DATA>(offset);
                os << data;
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
            case SMBIOS_PCIE_INFO_DATA::ID: {
                const auto data = parser.read_struct<SMBIOS_PCIE_INFO_DATA>(offset);
                os << data;
                break;
            }
            default:
                offset += header.length;
                /* Get past trailing string-list - double-null */
                while (offset < parser.buf_size && *(reinterpret_cast<const uint16_t *> (parser.buf.get() + offset)) != 0) {
                    offset++;
                }
                offset += 2;
                break;
        }
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const StructEnhanced<SMBIOS_SYSTEM_INFO_DATA> &s) {
    os << "SMBIOS_SYSTEM_INFO_DATA\t@" << s.header.handle << "\n";
    os << "\tManufacturer   : " << s.get_string(s.data.manufacturer)
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

std::ostream &operator<<(std::ostream &os, const StructEnhanced<SMBIOS_PCIE_INFO_DATA> &s) {
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

std::ostream &operator<<(std::ostream &os, const StructEnhanced<SMBIOS_STORAGE_INFO_DATA> &s) {
    os << "HDD_INFO_DATA\t@" << s.header.handle << "\n";
    os << "\tPciClassCode    : 0x" << std::hex << static_cast<uint32_t> (s.data.PciClassCode)
       << "\n\tVendorID        : 0x" << std::hex << s.data.VendorID
       << "\n\tDeviceID        : 0x" << std::hex << s.data.DeviceID
       << "\n\tSubVendorID     : 0x" << std::hex << s.data.SubVendorID
       << "\n\tSubDeviceID     : 0x" << std::hex << s.data.SubDeviceID
       << "\n\tHDDIndex        : " << std::dec << static_cast<uint32_t> (s.data.RsaHddInfoData.HDDIndex)
       << "\n\tHDDName         : " << std::string(reinterpret_cast<const char*>(s.data.RsaHddInfoData.HDDName),
                                                  HDD_INFO_DATA::HDD_NAME_STRING_SIZE)
       << "\n\tHDDType         : 0x" << std::dec << static_cast<uint32_t> (s.data.RsaHddInfoData.HDDType)
       << "\n\tHDDSizeInGB     : " << std::dec << s.data.RsaHddInfoData.HDDSizeInGB
       << std::endl;
    return os;
}

std::ostream &operator<<(std::ostream &os, const StructEnhanced<SMBIOS_STORAGE_INFO_DATA_V2> &s) {
    os << "HDD_INFO_DATA\t@" << s.header.handle << "\n";
    os << "\tPciClassCode    : 0x" << std::hex << static_cast<uint32_t> (s.data.PciClassCode)
       << "\n\tPciSubClassCode : 0x" << std::hex << static_cast<uint32_t> (s.data.PciSubClassCode)
       << "\n\tVendorID        : 0x" << std::hex << s.data.VendorID
       << "\n\tDeviceID        : 0x" << std::hex << s.data.DeviceID
       << "\n\tSubVendorID     : 0x" << std::hex << s.data.SubVendorID
       << "\n\tSubDeviceID     : 0x" << std::hex << s.data.SubDeviceID
       << "\n\tHDDIndex        : " << std::dec << static_cast<uint32_t> (s.data.RsaHddInfoData.HDDIndex)
       << "\n\tHDDName         : " << std::string(reinterpret_cast<const char*>(s.data.RsaHddInfoData.HDDName),
                                                  HDD_INFO_DATA_V2::HDD_NAME_STRING_SIZE)
       << "\n\tHDDSerialNumber : " << std::string(reinterpret_cast<const char*>(s.data.RsaHddInfoData.HDDSerialNumber),
                                                  HDD_INFO_DATA_V2::HDD_SERIAL_NUMBER_SIZE)
       << "\n\tHDDType         : 0x" << std::dec << static_cast<uint32_t> (s.data.RsaHddInfoData.HDDType)
       << "\n\tHDDSizeInGB     : " << std::dec << s.data.RsaHddInfoData.HDDSizeInGB
       << std::endl;
    return os;
}

std::ostream &operator<<(std::ostream &os, const StructEnhanced<SMBIOS_NIC_INFO_DATA> &s) {
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

}
}
