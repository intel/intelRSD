/*!
 * @brief Implementation of GetSensorReading Request/Response.
 *
 * @copyright Copyright (c) 2015-2019 Intel Corporation.
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
 * @file command/generic/get_sensor_reading.cpp
 */

#include "ipmi/command/generic/get_sensor_reading.hpp"
#include "ipmi/command/generic/enums.hpp"

using namespace ipmi;
using namespace ipmi::command::generic;

namespace {

constexpr std::uint8_t READING_UNAVAILABLE_MASK = 0x20;
constexpr std::uint8_t SCANNING_DISABLED_MASK = 0x40;

}


request::GetSensorReading::GetSensorReading() :
        Request(generic::NetFn::SENSOR_EVENT, generic::Cmd::GET_SENSOR_READING) {}


request::GetSensorReading::~GetSensorReading() {}


void request::GetSensorReading::pack(IpmiInterface::ByteBuffer& data) const {
    data.push_back(m_sensor_number);
}


response::GetSensorReading::GetSensorReading() :
        Response(generic::NetFn::SENSOR_EVENT + 1, generic::Cmd::GET_SENSOR_READING, MIN_RESPONSE_SIZE) {}


response::GetSensorReading::~GetSensorReading() {}


uint8_t response::GetSensorReading::get_sensor_reading() const {
    return m_sensor_reading;
}


std::uint8_t response::GetSensorReading::get_data_1() const {
    return m_data1;
}


std::uint8_t response::GetSensorReading::get_data_2() const {
    return m_data2;
}


void response::GetSensorReading::unpack(const IpmiInterface::ByteBuffer& data) {
    m_sensor_reading = data[OFFSET_DATA];
    m_reading_validity = data[OFFSET_READING_VALIDITY];
    if (data.size() > MIN_RESPONSE_SIZE + 1) {
        m_data1 = data[OFFSET_DATA_1];
    }
    if (data.size() > MIN_RESPONSE_SIZE + 2) {
        m_data2 = data[OFFSET_DATA_2];
    }
}


bool response::GetSensorReading::is_valid_reading() const {
    return !(m_reading_validity & READING_UNAVAILABLE_MASK);
}


bool response::GetSensorReading::is_scanning_disabled() const {
    return !(m_reading_validity & SCANNING_DISABLED_MASK);
}
