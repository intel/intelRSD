/*!
 * @brief ReadPcieCableEepromData command interface.
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
 * @file read_pcie_cable_eeprom_data.cpp
 */

#include "ipmi/command/sdv/rsd/read_pcie_cable_eeprom_data.hpp"
#include "ipmi/command/sdv/rsd/constants.hpp"



using namespace ipmi;
using namespace ipmi::command::sdv::rsd;


request::ReadPcieCableEepromData::ReadPcieCableEepromData() : Request(NetFn::GROUP_EXTENSION,
                                                                      Cmd::READ_PCIE_CABLE_EEPROM_DATA) {}


request::ReadPcieCableEepromData::~ReadPcieCableEepromData() {}


void request::ReadPcieCableEepromData::pack(IpmiInterface::ByteBuffer& data) const {
    std::uint8_t pcie_slot_it_lsb = static_cast<std::uint8_t>(m_pcie_slot_id & 0xff);
    std::uint8_t pcie_slot_it_msb = static_cast<std::uint8_t>((m_pcie_slot_id >> 8) & 0xff);

    data.push_back(static_cast<uint8_t>(GroupExtension::RACKSCALE));
    data.push_back(pcie_slot_it_lsb);
    data.push_back(pcie_slot_it_msb);
    data.push_back(m_cable_port_index);
    data.push_back(m_upper_memory_page_select);
    data.push_back(m_reading_data_offset);
    data.push_back(m_reading_data_length);
}


response::ReadPcieCableEepromData::ReadPcieCableEepromData() : Response(NetFn::GROUP_EXTENSION + 1,
                                                                        Cmd::READ_PCIE_CABLE_EEPROM_DATA,
                                                                        RESPONSE_MINIMUM_SIZE) {}


response::ReadPcieCableEepromData::~ReadPcieCableEepromData() {}


void response::ReadPcieCableEepromData::unpack(const IpmiInterface::ByteBuffer& data) {
    if (static_cast<uint8_t>(GroupExtension::RACKSCALE) == data[constants::GROUP_EXTENSION_ID_OFFSET]) {
        m_is_rackscale_extension_byte_present = true;
    }

    m_read_data_length = data[READ_DATA_LENGTH_OFFSET];
    m_cable_memory_data.resize(m_read_data_length);
    std::copy(data.begin() + CABLE_MEMORY_DATA_OFFSET, data.end(), m_cable_memory_data.begin());
}
