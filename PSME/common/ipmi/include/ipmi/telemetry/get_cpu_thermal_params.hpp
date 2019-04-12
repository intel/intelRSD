/*!
 * @brief GetCpuThermalParams.
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
 * @file ipmi/telemetry/get_cpu_thermal_params.hpp
 */

#pragma once

#include "ipmi/ipmi_controller.hpp"
#include "ipmi/command/sdv/enums.hpp"
#include "ipmi/command/generic/raw_request.hpp"

#include <array>

namespace ipmi {
namespace sdv {

/*!
 * @brief Retrieves CPU thermal parameters.
 */
template<std::uint8_t CPU_COUNT = 2>
class CpuThermalParams {
public:
    /*! Constructor */
    CpuThermalParams() {
        m_readings.fill({0x0, ipmi::command::sdv::NO_READING,
                         ipmi::command::sdv::NO_READING,
                         ipmi::command::sdv::NO_READING});
    }

    /*! @return Number of CPU instances */
    static constexpr int num_cpus() {
        return CPU_COUNT;
    }

    /*
     * @param cpu_no CPU instance number (zero based up to num_cpus())
     * @return TJmax for given CPU instance.
     */
    std::uint8_t get_cpu_tjmax(int cpu_no) {
        return m_readings.at(cpu_no).tj_max;
    }

    /*
     * @param cpu_no CPU instance number (zero based up to num_cpus())
     * @return TControl for given CPU instance.
     */
    std::uint8_t get_cpu_tcontrol(int cpu_no) {
        return m_readings.at(cpu_no).tcontrol;
    }

    /*
     * @brief Updates thermal parameters readings
     * @param ipmi IpmiController
     * @param bridge Bridge to Node Manager
     */
    void update_readings(ipmi::IpmiController& ipmi, const ipmi::BridgeInfo& bridge) {
        try {
            for (auto i = 0u; i < m_readings.size(); ++i) {
                m_readings[i] = read(i, ipmi, bridge);
            }
        }
        catch(const NodeBusyError& e) {
            log_warning("telemetry", e.what());
        }
        catch(const CommandNotSupportedInPresentStateError&) {
            m_readings.fill({0x0, ipmi::command::sdv::NO_READING,
                         ipmi::command::sdv::NO_READING,
                         ipmi::command::sdv::NO_READING});
        }
    }

private:

    #pragma pack(push, 1)
    struct Reading {
        std::uint8_t _placeholder_;
        std::uint8_t tcontrol;
        std::uint8_t tj_max;
        std::uint8_t tj_max_tcc_offset;
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct Response {
        std::uint8_t iana[4];
        std::uint8_t status;
        Reading reading;
    };
    #pragma pack(pop)

    std::array<Reading, CPU_COUNT> m_readings{{}};

    Reading read(int cpu_no, ipmi::IpmiController& ipmi, const ipmi::BridgeInfo& bridge) {
        constexpr const char* CMD_NAME = "GetCpuThermalParams";
        ipmi::IpmiInterface::ByteBuffer msg{{0x2e, 0x41, 0x57, 0x01, 0x00, 0x30, 0x05, 0x05, 0xa1, 0x00, 0x10, 0x00, 0x00}};
        if (cpu_no) {
            msg[5] = std::uint8_t(0x30 + cpu_no);
        }
        ipmi::command::generic::request::RawRequest req{CMD_NAME, std::move(msg)};
        ipmi::command::generic::response::RawResponse rsp{CMD_NAME};
        ipmi.send(req, bridge, rsp);
        return rsp.convert_to_struct<Response>().reading;
    }
};

}
}
