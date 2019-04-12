/*!
 * @brief Intel Optane DC Persistent Memory data entry point interface
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
 * @file dcpmem_entry_point.cpp
 */

#include "pmem/dcpmem_entry_point.hpp"
#include "pmem/dcpmem_command_entry_point.hpp"
#include "pmem/dcpmem_parser.hpp"

namespace pmem {
namespace parser {

DcpmemEntryPoint::~DcpmemEntryPoint() {}

DcpmemEntryPoint::Exception::~Exception() {}

DcpmemEntryPoint::Ptr DcpmemEntryPoint::create(const std::uint8_t* buf, const size_t buf_size) {
    if (buf_size < sizeof(DcpmemCommandEntryPoint::EntryPointStructure)) {
        throw Exception("Invalid DCPMEM command entry point size");
    }

    return DcpmemEntryPoint::Ptr(new DcpmemCommandEntryPoint(buf, buf_size));
}

std::uint64_t DcpmemEntryPoint::get_table_data_offset(const std::string&) const {
    throw Exception("Method not implemented: get_table_data_offset()");
}

uint64_t DcpmemEntryPoint::get_table_data_end_offset(const std::string&) const {
    throw Exception("Method not implemented: get_table_data_end_offset()");
}

void DcpmemEntryPoint::set_struct_table_address(std::uint64_t) {
    throw Exception("Method not implemented: set_struct_table_address()");
}

void DcpmemEntryPoint::set_struct_table_end_address(uint64_t) {
    throw Exception("Method not implemented: set_struct_table_end_address()");
}

}
}