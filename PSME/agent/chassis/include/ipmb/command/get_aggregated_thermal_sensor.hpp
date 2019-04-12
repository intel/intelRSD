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
 * @file ipmb/command/get_aggregated_thermal_sensor.hpp
 * @brief GetAggregatedThermalSensor IPMI command handler for IPMB.
 * */

#pragma once
#include <ipmb/command/command.hpp>
#include "ipmb/command/thermal_sensor_response.hpp"

/*! Agent namesapce */
namespace agent {
/*! Chassis namesapce */
namespace chassis {
/*! Ipmb namesapce */
namespace ipmb {
/*! Command namesapce */
namespace command {

/*! GetAggregatedThermalSensor command */
class GetAggregatedThermalSensor final : public Command {
public:
    GetAggregatedThermalSensor() { }
    GetAggregatedThermalSensor(const GetAggregatedThermalSensor &) = default;
    GetAggregatedThermalSensor &operator=(const GetAggregatedThermalSensor &) = default;

    ~GetAggregatedThermalSensor();

    /*!
     * Unpacks IPMI Message
     * @param msg IPMI Message
     * */
    void unpack(IpmiMessage &msg);

    /*!
     * Packs IPMI Message
     * @param msg IPMI Message
     * */
    void pack(IpmiMessage &msg);
private:
    static const constexpr uint8_t CMD_RESPONSE_DATA_LENGHT = 1 + sizeof(ThermalSensorIpmbResponse);
    void make_response(ThermalSensorIpmbResponse& response);
};

}
}
}
}


