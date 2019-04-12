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
 * */

#pragma once
#include <ipmb/command/drawer_power_response.hpp>
#include <ipmb/command/thermal_sensor_response.hpp>
#include "agent-framework/generic/singleton.hpp"

#include <mutex>

namespace agent {
namespace chassis {
namespace ipmb {

/*! Global object for storing ipmb cached data */
class RmmGlobalData : public agent_framework::generic::Singleton<RmmGlobalData> {
public:
    /*!
     * @brief Set thermal sensor data
     * @param thermal_sensor Thermal sensor object
     */
    void set_thermal_sensors_res(const ThermalSensorIpmbResponse& thermal_sensor) {
        std::lock_guard<std::mutex> lock{m_mutex};
        m_thermal_sensor_res = thermal_sensor;
    }

    /*!
     * @brief Get thermal sensor data
     * @return Thermal sensor object
     */
    ThermalSensorIpmbResponse get_thermal_sensors_res() const {
        std::lock_guard<std::mutex> lock{m_mutex};
        return m_thermal_sensor_res;
    }

    /*!
     * @brief Set drawer power data
     * @param drawer_power Drawer power object
     */
    void set_drawer_power(const DrawerPowerResponse& drawer_power) {
        std::lock_guard<std::mutex> lock{m_mutex};
        m_drawer_power = drawer_power;
    }

    /*!
     * @brief Get drawer power data
     * @return Drawer power object
     */
    DrawerPowerResponse get_drawer_power() const {
        std::lock_guard<std::mutex> lock{m_mutex};
        return m_drawer_power;
    }

    ~RmmGlobalData();

private:
    mutable std::mutex m_mutex{};
    DrawerPowerResponse m_drawer_power{};
    ThermalSensorIpmbResponse m_thermal_sensor_res{};
};

}
}
}

