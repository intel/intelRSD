/*!
 * @brief GetCupsData.
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
 * @file ipmi/telemetry/get_cups_data.hpp
 */

#pragma once

#include "ipmi/ipmi_controller.hpp"
#include "ipmi/command/sdv/enums.hpp"
#include "ipmi/command/generic/raw_request.hpp"

namespace ipmi {
namespace sdv {

/*!
 * @brief Retrieves CUPS readings.
 */
class GetCupsData {
public:

    /*
     * @return CPU bandwidth percent.
     */
    std::uint8_t get_cpu_bandwidth_percent() const {
        return m_reading.cpu_cups_dynamic_load_factor;
    }

    /*
     * @return Memory bandwidth percent.
     */
    std::uint8_t get_memory_bandwidth_percent() const {
        return m_reading.mem_cups_dynamic_load_factor;
    }

    /*
     * @return IO bandwidth percent.
     */
    std::uint8_t get_io_bandwidth_percent() const {
        return m_reading.io_cups_dynamic_load_factor;
    }

    /*
     * @brief Updates CUPS readings
     * @param ipmi IpmiController
     * @param bridge Bridge to Node Manager
     */
    void update_readings(ipmi::IpmiController& ipmi, const ipmi::BridgeInfo& bridge) {
        m_reading = read(ipmi, bridge);
    }

private:
    #pragma pack(push, 1)
    struct Reading {
        std::uint8_t cpu_cups_dynamic_load_factor;
        std::uint8_t cpu_cups_dynamic_load_factor_2;
        std::uint8_t mem_cups_dynamic_load_factor;
        std::uint8_t mem_cups_dynamic_load_factor_2;
        std::uint8_t io_cups_dynamic_load_factor;
        std::uint8_t io_cups_dynamic_load_factor_2;
    };

    struct Response {
        std::uint8_t iana[3];
        Reading reading;
    };
    #pragma pack(pop)

    Reading read(ipmi::IpmiController& ipmi, const ipmi::BridgeInfo& bridge) {
        constexpr const char* CMD_NAME = "GetCupsData";
        ipmi::command::generic::request::RawRequest req{CMD_NAME, {0x2e, 0x65, 0x57, 0x01, 0x00, 0x02}};
        ipmi::command::generic::response::RawResponse rsp{CMD_NAME};
        ipmi.send(req, bridge, rsp);
        return rsp.convert_to_struct<Response>().reading;
    }

    Reading m_reading{0u,0u,0u,0u,0u,0u};
};

}
}
