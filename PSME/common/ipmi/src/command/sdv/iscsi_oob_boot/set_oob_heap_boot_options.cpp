/*!
 * @brief Set OOB Heap Boot Options Command Implementation.
 *
 * @copyright Copyright (c) 2016-2019 Intel Corporation
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
 * @file set_oob_heap_boot_option.cpp
 */

#include "ipmi/command/sdv/iscsi_oob_boot/set_oob_heap_boot_options.hpp"



using namespace ipmi;
using namespace ipmi::command::sdv;


request::SetOobHeapBootOptions::SetOobHeapBootOptions() :
    request::SetOobBootOptions(request::SetOobBootOptions::ParameterType::HEAP) {
}


request::SetOobHeapBootOptions::~SetOobHeapBootOptions() {}


void request::SetOobHeapBootOptions::pack(std::vector<uint8_t>& data) const {

    data.push_back(uint8_t(get_parameter_id()));
    push_uint16(data, uint16_t(m_data.size()));
    push_uint16(data, m_offset);
    for (uint8_t byte : m_data) {
        data.push_back(byte);
    }
}


response::SetOobHeapBootOptions::SetOobHeapBootOptions() :
    response::SetOobBootOptions() {}


response::SetOobHeapBootOptions::~SetOobHeapBootOptions() {}


void response::SetOobHeapBootOptions::unpack(const std::vector<uint8_t>&) { }
