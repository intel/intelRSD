/*!
 * @brief ACPI table entry point interface
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
 * @file acpi_entry_point.cpp
 */

#include "acpi/acpi_entry_point.hpp"
#include "acpi/acpi_table_entry_point.hpp"
#include "acpi/acpi_parser.hpp"

namespace acpi {
namespace parser {

AcpiEntryPoint::~AcpiEntryPoint() {}

AcpiEntryPoint::Exception::~Exception() {}

AcpiEntryPoint::Ptr AcpiEntryPoint::create(const std::uint8_t* buf, const size_t buf_size) {
    if (buf_size < sizeof(AcpiTableEntryPoint::EntryPointStructure)) {
        throw Exception("Invalid ACPI table size");
    }

    return AcpiEntryPoint::Ptr(new AcpiTableEntryPoint(buf, buf_size));
}

}
}