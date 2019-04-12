/*!
 * @brief GetCpuAndDimmTemperature.
 *
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
 * @file ipmi/telemetry/get_cpu_and_dimm_temperature.hpp
 */

#pragma once

#include "ipmi/ipmi_controller.hpp"
#include "ipmi/command/sdv/enums.hpp"
#include "ipmi/command/sdv/nm/errors.hpp"
#include "ipmi/command/sdv/nm/get_cpu_and_memory_temperature.hpp"

#include <algorithm>
#include <array>

namespace ipmi {
namespace sdv {

template<typename T>
constexpr int popcount(T x) {
    return (x == 0) ? 0 : 1 + popcount(x & (x - 1));
}

using ipmi::command::sdv::request::MEMORY_MASK;
using ipmi::command::sdv::request::CHANNEL_SET;
using ipmi::command::sdv::request::CPU_MASK;
using ipmi::command::sdv::request::CPU_SET;

/*!
 * @brief Helper class for executing GetCpuAndMemoryTemperature command.
 *
 * Allows to specify configuration specific for given platform.
 */
template<std::uint16_t DIMMS_MASK,
        CHANNEL_SET CHANNELS = CHANNEL_SET::CHANNELS_0_TO_3,
        std::uint8_t CPUS_MASK = CPU_MASK::CPU0 | CPU_MASK::CPU1,
        std::uint8_t CPUS_SET = CPU_SET::CPU0_TO_3>
class GetCpuAndDimmTemperature {
public:
    /*! @return Number of readings */
    static constexpr int num_readings() {
        return num_cpus() * (1 + num_dimms_per_cpu());
    }

    /*! @return Number of CPU instances */
    static constexpr int num_cpus() {
        return popcount(CPUS_MASK);
    }

    /*! @return Number of DIMMS per CPU instance */
    static constexpr int num_dimms_per_cpu() {
        return popcount(DIMMS_MASK);
    }

    /*! Constructor */
    GetCpuAndDimmTemperature() {
        m_readings.fill(ipmi::command::sdv::NO_READING);
    }

    /*!
     * @brief Returns margin to throttle for given cpu instance.
     * @param cpu_no CPU instance number (zero based up to num_cpus())
     * @return margin to throttle for given cpu instance.
     */
    std::uint8_t get_cpu_margin_to_throttle(int cpu_no) const {
        return m_readings.at(cpu_no);
    }

    /*!
     * @brief Returns selected dimm temperature in Celsius degree.
     * @param cpu_no CPU instance number (zero based up to num_cpus())
     * @param dimm_no DIMM instance number (zero based up to num_dimms_per_cpu())
     * @return dimm temperature in Celsius degree.
     */
    std::uint8_t get_dimm_temperature(int cpu_no, int dimm_no) const {
        const auto idx = num_cpus() + cpu_no * num_dimms_per_cpu() + dimm_no;
        return m_readings.at(idx);
    }

    /*
     * @brief Updates temperature readings
     * @param ipmi IpmiController
     * @param bridge Bridge to Node Manager
     */
    void update_readings(ipmi::IpmiController& ipmi, const ipmi::BridgeInfo& bridge) {
        using namespace ipmi::command::sdv;
        request::GetCpuAndMemoryTemperature req{};
        response::GetCpuAndMemoryTemperature rsp{};
        req.select_cpu_set(CPUS_SET);
        req.set_cpu_mask(CPUS_MASK);
        req.select_memory_channels(CHANNELS);
        for (int cpu = 0; cpu < num_cpus(); ++cpu) {
            req.set_memory_channel_mask_for_cpu(cpu, DIMMS_MASK);
        }
        try {
            ipmi.send(req, bridge, rsp);
            const auto readings = rsp.get_readings();
            if (readings.size() != num_readings()) {
                m_readings.fill(NO_READING);
                throw ipmi::ResponseLengthError(rsp, num_readings(), m_readings.size());
            }
            std::copy_n(std::make_move_iterator(readings.begin()), num_readings(), m_readings.begin());
        }
        catch(const NodeBusyError& e) {
            log_warning("telemetry", e.what());
        }
        catch(const InvalidPlatformState&) {
            m_readings.fill(NO_READING);
        }
    }

private:
    std::array<std::uint8_t, num_readings()> m_readings{{}};
};

}
}
