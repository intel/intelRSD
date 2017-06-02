/*!
 * @brief Get OOB Heap Boot Options Command Implementation.
 *
 * @copyright Copyright (c) 2016-2017 Intel Corporation
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
 * @header{Files}
 * @file get_oob_heap_boot_option.cpp
 */

#include "ipmi/command/sdv/get_oob_heap_boot_options.hpp"



using namespace ipmi;
using namespace ipmi::command::sdv;


request::GetOobHeapBootOptions::GetOobHeapBootOptions(uint16_t offset, uint16_t size) :
    request::GetOobBootOptions(request::GetOobBootOptions::ParameterType::HEAP) {

    m_offset = offset;
    m_size = size;
}


request::GetOobHeapBootOptions::~GetOobHeapBootOptions() {}


void request::GetOobHeapBootOptions::pack(std::vector<uint8_t>& data) const {

    data.push_back(get_parameter_id());
    data.push_back(0);
    push_uint16(data, get_offset());
    push_uint16(data, get_size());
}


response::GetOobHeapBootOptions::GetOobHeapBootOptions() :
    response::GetOobBootOptions() {}


response::GetOobHeapBootOptions::~GetOobHeapBootOptions() {}


void response::GetOobHeapBootOptions::unpack(const std::vector<uint8_t>& data) {
    m_completion_code = COMPLETION_CODE(data[OFFSET_COMPLETION_CODE]);

    if (ERROR_DATA_SIZE == data.size()) {
        return;
    }

    size_t data_size = data.size();

    if (data_size < MIN_RESPONSE_SIZE) {
        throw std::runtime_error(("Cannot unpack OOB Heap Boot Options response. Data length too short."
            " Expected at least: ")
                                 + std::to_string(MIN_RESPONSE_SIZE)
                                 + " Received: "
                                 + std::to_string(data.size()));
    }

    set_max_length(pull_uint16(data, OFFSET_MAX_SIZE));
    set_length(pull_uint16(data, OFFSET_CUR_SIZE));

    std::vector<uint8_t>::const_iterator first = data.begin() + OFFSET_HEAP_OFFSET;
    std::vector<uint8_t>::const_iterator last = data.end();
    std::vector <uint8_t> heap(first, last);

    if (heap.size() != get_length()) {
        throw std::runtime_error("Oob Heap Boot Options response mismatch. Data length retrieved ("
                                 + std::to_string(heap.size())
                                 + ") doesn't match length on record ("
                                 + std::to_string(unsigned(get_length()))
                                 + ")");
    }

    set_heap(heap);
}
