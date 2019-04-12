/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 *
 * @file get_processor_info.cpp
 *
 * @brief Get Processor Info request and response.
 * */
#include "ipmi/command/sdv/get_processor_info.hpp"
#include "ipmi/command/sdv/enums.hpp"

using namespace ipmi;
using namespace ipmi::command::sdv;

request::GetProcessorInfo::GetProcessorInfo() : Request(sdv::NetFn::QUANTA, sdv::Cmd::GET_PROCESSOR_INFO) {}
request::GetProcessorInfo::~GetProcessorInfo() {}

void request::GetProcessorInfo::pack(std::vector<std::uint8_t>& data) const {
    data.push_back(m_cpu_id);
}

response::GetProcessorInfo::GetProcessorInfo() : Response(sdv::NetFn::QUANTA, sdv::Cmd::GET_PROCESSOR_INFO, RESPONSE_SIZE) {}
response::GetProcessorInfo::~GetProcessorInfo() {}

void response::GetProcessorInfo::unpack(const std::vector<std::uint8_t>& data) {
    m_cpu_type = CPU_TYPE(data[OFFSET_CPU_TYPE]);
    m_cpu_frequency = extract_cpu_frequency(data);
    m_presence = data[OFFSET_CPU_PRESENCE] == 1 ? true : false;
}

std::uint16_t response::GetProcessorInfo::extract_cpu_frequency(const std::vector<std::uint8_t>& data) const {
    std::uint16_t freq = std::uint16_t((data[OFFSET_CPU_FREQUENCY + 1] << 8) | (data[OFFSET_CPU_FREQUENCY]));
    return freq;
}

const std::string& response::GetProcessorInfo::get_cpu_type_name() const {
    CPU_TYPE type = get_cpu_type();

    if (m_cpu_type_name.end() == m_cpu_type_name.find(type)) {
        type = CPU_TYPE_UNKNOWN;
    }

    return m_cpu_type_name.at(type);
}
