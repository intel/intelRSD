/*!
 * @brief Implementation of the iSCSI MDR entry point interface.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
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
 * @file iscsi_mdr_entry_point.cpp
 */

#include "iscsi/iscsi_mdr_entry_point.hpp"
#include "iscsi/iscsi_mdr_10_entry_point.hpp"
#include "iscsi/structs/iscsi_mdr_version.hpp"

namespace iscsi {
namespace parser {

IscsiMdrEntryPoint::Exception::~Exception() {
}

IscsiMdrEntryPoint::~IscsiMdrEntryPoint() {
}

IscsiMdrEntryPoint::Ptr IscsiMdrEntryPoint::create(const std::uint8_t* buf, const size_t buf_size) {
    if (buf_size < sizeof(structs::ISCSI_MDR_VERSION)) {
        throw Exception("Unable to determine iSCSI MDR version");
    }

    return IscsiMdrEntryPoint::Ptr(new IscsiMdr10EntryPoint(buf, buf_size));
}

std::uint64_t IscsiMdrEntryPoint::get_table_data_offset(const std::string&) const {
    throw Exception("Method not implemented: get_table_data_offset()");
}

uint64_t IscsiMdrEntryPoint::get_table_data_end_offset(const std::string&) const {
    throw Exception("Method not implemented: get_table_data_end_offset()");
}

void IscsiMdrEntryPoint::set_struct_table_address(std::uint64_t) {
    throw Exception("Method not implemented: set_struct_table_address()");
}

void IscsiMdrEntryPoint::set_struct_table_end_address(std::uint64_t) {
    throw Exception("Method not implemented: set_struct_table_end_address()");
}

}  // namespace parser
}  // namespace iscsi
