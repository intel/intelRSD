/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 *
 * @file get_aggregated_thermal_sensor.cpp
 *
 * @brief GetAggregatedThermalSensor IPMI command request and response implementation.
 * */

#include "ipmi/command/sdv/rmm/get_aggregated_thermal_sensor.hpp"
#include "ipmi/command/sdv/enums.hpp"

using namespace ipmi;
using namespace ipmi::command::sdv;

request::GetAggregatedThermalSensor::GetAggregatedThermalSensor():
    Request(NetFn::INTEL, Cmd::GET_AGGREGATED_THERMAL_SENSOR) {}

void request::GetAggregatedThermalSensor::pack(IpmiInterface::ByteBuffer&) const {
}

response::GetAggregatedThermalSensor::GetAggregatedThermalSensor():
    Response(NetFn::INTEL + 1, Cmd::GET_AGGREGATED_THERMAL_SENSOR, MIN_RESPONSE_SIZE) {}

void response::GetAggregatedThermalSensor::unpack(const IpmiInterface::ByteBuffer& data) {
    /*  Two bytes per entry starting from DATA_OFFSET position*/
    m_slots = uint8_t((data.size() - DATA_OFFSET) / ENTRY_SIZE);
    for (uint8_t slot_id = 0; slot_id < m_slots; ++slot_id) {
        m_temperature_on_slot[slot_id] = data[DATA_OFFSET + slot_id * ENTRY_SIZE + TEMPERATURE_OFFSET];
    }
}
