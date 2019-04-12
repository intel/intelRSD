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
 * @file rmm/ptas_reader/ptas_reader.hpp
 */

#pragma once

#include "ptas_reader/ptas_reader_interface.hpp"

#include <tuple>
#include <vector>
#include <memory>
#include <atomic>

struct PTAS_Statistic;

namespace agent {
namespace rmm {
namespace ptas {

class PtasReader : public PtasReaderInterface {

public:

    /*!
     * @brief Initializes ptas
     * @param port Ptas port
     * @param timeout Time to wait for response, seconds
     * @param fan_count Total number of fans
     * @param drawer_count Total number of drawers
     */
    PtasReader(int port, int timeout, int fan_count, int drawer_count);

    PtasReader(const PtasReader&) = delete;
    PtasReader& operator=(const PtasReader&) = delete;
    PtasReader(PtasReader&&) = delete;
    PtasReader& operator=(PtasReader&&) = delete;

    virtual ~PtasReader() {}

    /*!
     * @brief Initializes PTAS message, to be called before add_fan_data or add_drawer_data calls
     * @param global_power Total power used by zone
     */
    virtual void reset_data(int global_power) override;

    /*!
     * @brief Adds fan data to the PTAS message
     * @param slot_id Fan slot id
     * @param fan_rpm Rpm of the fan
     */
    virtual void add_fan_data(unsigned slot_id, unsigned fan_rpm) override;

    /*!
     * @brief Adds drawer data to the PTAS message
     * @param slot_id Drawer slot_id
     * @param power Power usage of the drawer
     * @param sled_data Sled data consisting of pairs {sled_id, sled_temperature}
     */
    virtual void add_drawer_data(unsigned slot_id, unsigned power, const std::vector<SledData>& sled_data) override;

    /*!
     * @brief Sends message to PTAS and receives response
     * @return True if succesful
     */
    virtual bool send_receive_data() override;

private:

    std::shared_ptr<PTAS_Statistic> m_statistics{};
    int m_timeout{1};

    /*! Sequence number needed for communication with ptas */
    static std::atomic<unsigned> sequence_number;
    static std::atomic<bool> is_initialized;
};

}
}
}
