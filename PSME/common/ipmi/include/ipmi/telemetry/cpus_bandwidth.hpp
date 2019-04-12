/*!
 * @brief CpusBandwidth.
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
 * @file ipmi/telemetry/cpus_bandwidth.hpp
 */

#pragma once

#include "ipmi/ipmi_controller.hpp"
#include "ipmi/command/generic/raw_request.hpp"
#include "ipmi/command/sdv/enums.hpp"

namespace ipmi {
namespace sdv {

class CpusBandwidth {
public:

    std::uint8_t get_bandwidth_percent() const {
        return bandwidth_percent;
    }

    void update_readings(ipmi::IpmiController& ipmi, const ipmi::BridgeInfo& bridge) {
        try {
            bandwidth_percent = read(ipmi, bridge);
        }
        catch(const NodeBusyError& e) {
            log_warning("telemetry", e.what());
        }
        catch(const CommandNotSupportedInPresentStateError&) {
            bandwidth_percent = ipmi::command::sdv::NO_READING;
        }
    }

private:
    #pragma pack(push, 1)
    struct Response {
        std::uint8_t iana[3];
        std::uint8_t bandwidth_percent;
        std::uint8_t _placeholder_;
    };
    #pragma pack(pop)

    std::uint8_t read(ipmi::IpmiController& ipmi, const ipmi::BridgeInfo& bridge) {
        constexpr const char* CMD_NAME = "GetCpusBandwidth";
        const ipmi::command::generic::request::RawRequest req{CMD_NAME, {0x2e, 0x65, 0x57, 0x01, 0x00, 0x01}};
        ipmi::command::generic::response::RawResponse rsp{CMD_NAME};
        ipmi.send(req, bridge, rsp);
        return rsp.convert_to_struct<Response>().bandwidth_percent;
    }

    std::uint8_t bandwidth_percent{ipmi::command::sdv::NO_READING};
};

}
}
