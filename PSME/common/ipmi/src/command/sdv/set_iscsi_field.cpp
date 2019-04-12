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
 * @file set_iscsi_field.cpp
 *
 * @brief Set iSCSI Field Command Implementation.
 * */

#include "ipmi/command/sdv/set_iscsi_field.hpp"
#include "ipmi/command/sdv/enums.hpp"

using namespace ipmi;
using namespace ipmi::command::sdv;

request::SetIscsiField::SetIscsiField() : Request(sdv::NetFn::INTEL, sdv::Cmd::SET_ISCSI_FIELD) {}
request::SetIscsiField::~SetIscsiField() {}

void request::SetIscsiField::pack(std::vector<std::uint8_t>& data) const {
    data.push_back(uint8_t(m_field_instance_type));
    data.push_back(uint8_t(m_data.size()));
    // Appends bytes from `m_data` to the end of the `data` vector
    append_vector(data, m_data);
}

void request::SetIscsiField::append_vector(std::vector<uint8_t>& output, const std::vector<std::uint8_t>& input) const {
    for(const auto& item: input) {
        output.push_back(item);
    }
}

response::SetIscsiField::SetIscsiField():
        Response(sdv::NetFn::INTEL, sdv::Cmd::SET_ISCSI_FIELD, RESPONSE_SIZE) {}
response::SetIscsiField::~SetIscsiField() {}

void response::SetIscsiField::unpack(const std::vector<std::uint8_t>& data) {
    m_field_max_byte_no = data[OFFSET_MAX_BYTES_NUMBER];
}
