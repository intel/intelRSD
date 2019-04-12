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
 * @file get_sensor_reading_factors.cpp
 * */
#include "ipmi/command/generic/get_sensor_reading_factors.hpp"
#include "ipmi/command/generic/enums.hpp"

extern "C" {
#include <ipmitool/ipmi_sdr.h>
}

namespace {

#pragma pack(push, 1)
struct SensorReadingFactors {
    std::uint8_t next_reading;
    std::uint16_t m_tol;
    std::uint32_t bacc;
};
#pragma pack(pop)

}

using namespace ipmi;
using namespace ipmi::command::generic;

request::GetSensorReadingFactors::GetSensorReadingFactors():
        Request(generic::NetFn::SENSOR_EVENT, generic::Cmd::GET_SENSOR_READING_FACTORS) {}
request::GetSensorReadingFactors::~GetSensorReadingFactors() {}

void request::GetSensorReadingFactors::pack(IpmiInterface::ByteBuffer& data) const {
    data.push_back(m_sensor_number);
    data.push_back(m_reading_byte);
}

response::GetSensorReadingFactors::GetSensorReadingFactors():
        Response(generic::NetFn::SENSOR_EVENT, generic::Cmd::GET_SENSOR_READING_FACTORS, RESPONSE_SIZE) {}
response::GetSensorReadingFactors::~GetSensorReadingFactors() {
}

void response::GetSensorReadingFactors::unpack(const IpmiInterface::ByteBuffer& data) {
    auto reading_factors = *reinterpret_cast<const SensorReadingFactors*>(data.data()+1);
    m_multiplier = __TO_M(reading_factors.m_tol);
    m_additive_offset =  __TO_B(reading_factors.bacc);
    m_exponent = __TO_B_EXP(reading_factors.bacc);
    m_result_exponent = __TO_R_EXP(reading_factors.bacc);
}
