/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * @file get_sensor_reading_factors.cpp
 * */
#include "ipmi/command/generic/get_sensor_reading_factors.hpp"
#include "ipmi/command/generic/enums.hpp"

using namespace ipmi;
using namespace ipmi::command::generic;

request::GetSensorReadingFactors::GetSensorReadingFactors():
        Request(generic::NetFn::SENSOR_EVENT, generic::Cmd::GET_SENSOR_READING_FACTORS) {}
request::GetSensorReadingFactors::~GetSensorReadingFactors() {}

void request::GetSensorReadingFactors::pack(std::vector<std::uint8_t>& data) const {
    data.push_back(m_sensor_number);
    data.push_back(m_reading_byte);
}

response::GetSensorReadingFactors::GetSensorReadingFactors():
        Response(generic::NetFn::SENSOR_EVENT, generic::Cmd::GET_SENSOR_READING_FACTORS, RESPONSE_SIZE) {}
response::GetSensorReadingFactors::~GetSensorReadingFactors() {
}

void response::GetSensorReadingFactors::unpack(const std::vector<std::uint8_t>& data) {

    if (!is_response_correct(data)) {
        return; // received only completion code, do not unpack.
    }

    m_multiplier = extract_multiplier(data);
}

uint16_t response::GetSensorReadingFactors::extract_multiplier(const std::vector<std::uint8_t>& data) const {
    return static_cast<std::uint16_t>((((data[OFFSET_MULTIPLIER_MSB] >> 6) & 0x3) << 8) |
                                 data[OFFSET_MULTIPLIER_LSB]);
}
