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
 * @file get_aggregated_health_sensor.cpp
 * @brief IPMI command GetAggregatedHealthSensor.
 * */

#include <ipmb/command/get_aggregated_health_sensor.hpp>
#include <ipmb/ipmi_message.hpp>
#include <ipmb/utils.hpp>

#include <logger/logger_factory.hpp>

using namespace agent::chassis::ipmb;
using namespace agent::chassis::ipmb::command;

GetAggregatedHealthSensor::~GetAggregatedHealthSensor() {}


void GetAggregatedHealthSensor::Response::add_data(IpmiMessage& msg) {
    auto data = msg.get_data();

    data[OFFSET_CC] = uint8_t(m_cc);
    data[OFFSET_STATUS] = get_health();

    msg.add_len(get_len());
}

void GetAggregatedHealthSensor::unpack(IpmiMessage& msg){
    log_debug(LOGUSR, "Unpacking Get Aggregated Health Sensor message.");
    msg.set_to_request();
    m_response.set_cc(CompletionCode::CC_OK);
}

void GetAggregatedHealthSensor::pack(IpmiMessage& msg){

    log_debug(LOGUSR, "Packing Get Aggregated Health Sensor message.");

    m_response.set_health(CMD_STATUS_OK);

    msg.set_cc(CompletionCode::CC_OK);
    msg.set_to_response();

    m_response.set_len(CMD_RESPONSE_DATA_LENGHT);
    m_response.add_data(msg);
}


