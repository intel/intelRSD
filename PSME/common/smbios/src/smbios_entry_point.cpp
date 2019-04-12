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

#include "smbios/smbios_entry_point.hpp"
#include "smbios/smbios_30_entry_point.hpp"
#include "smbios/smbios_21_entry_point.hpp"
#include "smbios/smbios_parser.hpp"


namespace {

constexpr size_t EPS_REVISION_OFFSET = 0x0a;

enum EntryPointVersion : uint8_t {
    SMBIOS_2_1_BASED = 0x00,
    SMBIOS_3_0_BASED = 0x01
};

}

namespace smbios {
namespace parser {

SmbiosEntryPoint::~SmbiosEntryPoint() {}

SmbiosEntryPoint::Exception::~Exception() {}


SmbiosEntryPoint::Ptr SmbiosEntryPoint::create(const std::uint8_t* buf, const size_t buf_size) {
    if (buf_size <= EPS_REVISION_OFFSET) {
        throw Exception("Entry Point Structure too small");
    }
    switch (buf[EPS_REVISION_OFFSET]) {
        case EntryPointVersion::SMBIOS_2_1_BASED:
            return SmbiosEntryPoint::Ptr(new Smbios21EntryPoint(buf, buf_size));
        case EntryPointVersion::SMBIOS_3_0_BASED:
            return SmbiosEntryPoint::Ptr(new Smbios30EntryPoint(buf, buf_size));
        default:
            throw Exception("Unknown Entry Point Structure Version: "
                    + std::to_string(int(buf[EPS_REVISION_OFFSET])));
    }
}

std::uint64_t SmbiosEntryPoint::get_table_data_offset(const std::string&) const {
    throw Exception("Method not implemented: get_table_data_offset()");
}

uint64_t SmbiosEntryPoint::get_table_data_end_offset(const std::string&) const {
    throw Exception("Method not implemented: get_table_data_end_offset()");
}

void SmbiosEntryPoint::set_struct_table_address(std::uint64_t) {
    throw Exception("Method not implemented: set_struct_table_address()");
}

void SmbiosEntryPoint::set_struct_table_end_address(uint64_t) {
    throw Exception("Method not implemented: set_struct_table_end_address()");
}

}
}
