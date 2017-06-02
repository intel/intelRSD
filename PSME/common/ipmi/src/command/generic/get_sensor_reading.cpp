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
 * @file get_sensor_reading.cpp
 * */
#include "ipmi/command/generic/get_sensor_reading.hpp"
#include "ipmi/command/generic/enums.hpp"

using namespace ipmi;
using namespace ipmi::command::generic;

request::GetSensorReading::GetSensorReading() :
        Request(generic::NetFn::SENSOR_EVENT, generic::Cmd::GET_SENSOR_READING) {}
request::GetSensorReading::~GetSensorReading() {}

void request::GetSensorReading::pack(std::vector<std::uint8_t>& data) const {
    data.push_back(m_sensor_number);
}

response::GetSensorReading::GetSensorReading() :
        Response(generic::NetFn::SENSOR_EVENT, generic::Cmd::GET_SENSOR_READING, RESPONSE_SIZE) {}
response::GetSensorReading::~GetSensorReading() {
}

uint8_t response::GetSensorReading::get_sensor_reading() const {
    return m_sensor_reading;
}

void response::GetSensorReading::unpack(const std::vector<std::uint8_t>& data) {

    if (!is_response_correct(data)) {
        return; // received only completion code, do not unpack.
    }

    m_sensor_reading = data[OFFSET_SENSOR_READING];
}
