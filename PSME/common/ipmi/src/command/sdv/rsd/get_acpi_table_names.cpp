/*!
 * @brief GetAcpiTableNames command interface.
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
 * @file get_acpi_table_names.cpp
 */

#include "ipmi/command/sdv/rsd/get_acpi_table_names.hpp"
#include "ipmi/command/sdv/enums.hpp"
#include "ipmi/command/sdv/rsd/constants.hpp"

using namespace ipmi;
using namespace ipmi::command::sdv::rsd;

request::GetAcpiTableNames::GetAcpiTableNames() : Request(sdv::NetFn::GROUP_EXTENSION, sdv::Cmd::GET_ACPI_TABLES_NAMES) {}
request::GetAcpiTableNames::~GetAcpiTableNames() {}

void request::GetAcpiTableNames::pack(IpmiInterface::ByteBuffer& data) const {
    data.push_back(static_cast<uint8_t>(GroupExtension::RACKSCALE));
}

response::GetAcpiTableNames::GetAcpiTableNames() : Response(sdv::NetFn::GROUP_EXTENSION, sdv::Cmd::GET_ACPI_TABLES_NAMES, RESPONSE_MINIMUM_SIZE) {}
response::GetAcpiTableNames::~GetAcpiTableNames() {}

void response::GetAcpiTableNames::unpack(const IpmiInterface::ByteBuffer& data) {
    if (static_cast<uint8_t>(GroupExtension::RACKSCALE) == data[constants::GROUP_EXTENSION_ID_OFFSET]) {
        m_is_rackscale_extension_byte_present = true;
    }

    m_data_length = uint8_t(data[DATA_LENGTH_OFFSET]);
    /* Table names start after first 3 bytes. Each string (table name) is 4 bytes long. */
    for (auto it = data.begin() + 3; it != data.end(); it += 4) {
        std::string name {};
        name.append(it, it + 4);

        m_acpi_table_names.push_back(name);
    }
}
