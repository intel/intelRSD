/*!
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
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
 * @file rmm/ptas_reader/ptas_reader_interface.hpp
 */

#pragma once

#include <tuple>
#include <vector>
#include <memory>

namespace agent {
namespace rmm {
namespace ptas {

class PtasReaderInterface {

public:

    using SledData = std::tuple<unsigned, unsigned char>;

    PtasReaderInterface() {}

    PtasReaderInterface(const PtasReaderInterface&) = default;
    PtasReaderInterface& operator=(const PtasReaderInterface&) = default;
    PtasReaderInterface(PtasReaderInterface&&) = default;
    PtasReaderInterface& operator=(PtasReaderInterface&&) = default;

    virtual ~PtasReaderInterface() {}

    /*!
     * @brief Initializes PTAS message, to be called before add_fan_data or add_drawer_data calls
     * @param global_power Total power used by zone
     */
    virtual void reset_data(int global_power) = 0;

    /*!
     * @brief Checks if reader data are valid
     * @return False if reader stores invalid data that should not be used
     */
    virtual bool is_data_valid() const {
        return m_is_data_valid;
    }

    /*!
     * @brief Adds fan data to the PTAS message
     * @param slot_id Fan slot id
     * @param fan_rpm Rpm of the fan
     */
    virtual void add_fan_data(unsigned slot_id, unsigned fan_rpm) = 0;

    /*!
     * @brief Adds drawer data to the PTAS message
     * @param slot_id Drawer slot_id
     * @param power Power usage of the drawer
     * @param sled_data Sled data consisting of pairs {sled_id, sled_temperature}
     */
    virtual void add_drawer_data(unsigned slot_id, unsigned power, const std::vector<SledData>& sled_data) = 0;

    /*!
     * @brief Returns volumetric airflow
     * @return Volumetric airflow
     */
    virtual double get_volumetric_airflow() const {
        return m_volumetric_airflow;
    }

    /*!
     * @brief Returns outlet temperature
     * @return Outlet temperature
     */
    virtual double get_outlet_temperature() const {
        return m_outlet_temperature;
    }

    /*!
     * @brief Sends message to PTAS and receives response
     * @return True if succesful
     */
    virtual bool send_receive_data() = 0;

protected:

    double m_volumetric_airflow{};
    double m_outlet_temperature{};
    bool m_is_data_valid{false};
};

}
}
}
