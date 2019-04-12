/*!
 * @brief GetCpuPeciPackageConfigData command implementation.
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
 * @file get_cpu_peci_package_config_data.cpp
 */

#include "ipmi/command/sdv/rsd/get_cpu_peci_package_config_data.hpp"
#include "ipmi/command/sdv/rsd/constants.hpp"
#include "ipmi/command/sdv/enums.hpp"

#include <algorithm>
#include <exception>



using namespace ipmi;
using namespace ipmi::command::sdv::rsd;


request::GetCpuPeciPackageConfigData::GetCpuPeciPackageConfigData() : Request(NetFn::GROUP_EXTENSION,
                                                                              Cmd::GET_CPU_PECI_PACKAGE_CONFIG_DATA) {}


request::GetCpuPeciPackageConfigData::~GetCpuPeciPackageConfigData() {}


void request::GetCpuPeciPackageConfigData::set_pcs_command_parameter(
    const std::vector<std::uint8_t>& pcs_command_parameter) {
    if (pcs_command_parameter.size() > 4) {
        throw std::runtime_error("PCS PECI data can be no longer than 4 bytes!");
    }
    else {
        std::copy(pcs_command_parameter.begin(), pcs_command_parameter.end(), m_pcs_command_parameter.begin());
    }
}


void request::GetCpuPeciPackageConfigData::pack(IpmiInterface::ByteBuffer& data) const {
    data.push_back(static_cast<uint8_t>(GroupExtension::RACKSCALE));
    data.push_back(m_node_index);
    data.push_back(m_cpu_index);
    data.push_back(m_pcs_command_index);
    // push reserved byte
    data.push_back(0);
    data.push_back(m_pcs_command_parameter[0]);
    data.push_back(m_pcs_command_parameter[1]);
    data.push_back(m_pcs_command_parameter[2]);
    data.push_back(m_pcs_command_parameter[3]);
}


response::GetCpuPeciPackageConfigData::GetCpuPeciPackageConfigData() : Response(NetFn::GROUP_EXTENSION + 1,
                                                                                Cmd::GET_CPU_PECI_PACKAGE_CONFIG_DATA,
                                                                                RESPONSE_SIZE) {}


void response::GetCpuPeciPackageConfigData::unpack(const IpmiInterface::ByteBuffer& data) {
    if (static_cast<uint8_t>(GroupExtension::RACKSCALE) == data[constants::GROUP_EXTENSION_ID_OFFSET]) {
        m_is_rackscale_extension_byte_present = true;
    }

    m_node_index = data[NODE_INDEX_OFFSET];
    m_cpu_index = data[CPU_INDEX_OFFSET];
    m_pcs_peci_data.push_back(data[PSC_PECI_DATA_OFFSET]);
    m_pcs_peci_data.push_back(data[PSC_PECI_DATA_OFFSET + 1]);
    m_pcs_peci_data.push_back(data[PSC_PECI_DATA_OFFSET + 2]);
    m_pcs_peci_data.push_back(data[PSC_PECI_DATA_OFFSET + 3]);
}
