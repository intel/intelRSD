/*!
 * @brief Implementation of temperature readers for Grantley platform
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
 * @file grantley_cpu_dimm_temperature_telemetry_reader.cpp
 */

#include "telemetry/grantley_cpu_dimm_temperature_telemetry_reader.hpp"
#include "ipmi/telemetry/get_cpu_and_dimm_temperature.hpp"
#include "ipmi/telemetry/get_cpu_thermal_params.hpp"


namespace {

using MEMORY_MASK = ipmi::command::sdv::request::MEMORY_MASK;
constexpr const std::uint16_t DIMMS_MASK =
          MEMORY_MASK::CHANNEL0_DIMM0 | MEMORY_MASK::CHANNEL0_DIMM1
        | MEMORY_MASK::CHANNEL1_DIMM0 | MEMORY_MASK::CHANNEL1_DIMM1
        | MEMORY_MASK::CHANNEL2_DIMM0 | MEMORY_MASK::CHANNEL2_DIMM1
        | MEMORY_MASK::CHANNEL3_DIMM0 | MEMORY_MASK::CHANNEL3_DIMM1;

using CpuDimmTemp = ipmi::sdv::GetCpuAndDimmTemperature<DIMMS_MASK>;
constexpr const auto NUM_CPUS = CpuDimmTemp::num_cpus();
constexpr const auto NUM_DIMMS_PER_CPU = CpuDimmTemp::num_dimms_per_cpu();
constexpr const auto NUM_DIMMS = NUM_DIMMS_PER_CPU * NUM_CPUS;

}

namespace telemetry {

class GrantleyContext : public TelemetryReader::Context {
public:
    GrantleyContext(ipmi::IpmiController& ctrl) : m_ctrl(ctrl) { }

    ipmi::sdv::CpuThermalParams<NUM_CPUS> cpus_thermal_params{};
    CpuDimmTemp cpus_dimms_temp{};

protected:
    bool update() override;

private:
    static const ipmi::BridgeInfo BRIDGE_TO_NM;
    ipmi::IpmiController& m_ctrl;
};

const ipmi::BridgeInfo GrantleyContext::BRIDGE_TO_NM = ipmi::BridgeInfo(0x2c, 0x06);

bool GrantleyContext::update() {
    cpus_thermal_params.update_readings(m_ctrl, BRIDGE_TO_NM);
    cpus_dimms_temp.update_readings(m_ctrl, BRIDGE_TO_NM);
    return true;
}


TelemetryReader::ReaderId GrantleyCpuDimmTemperatureTelemetryReader::assigned_reader_id() {
    static ReaderId reader_id = TelemetryReader::assign_reader_id();
    return reader_id;
}

TelemetryReader::Context::Ptr GrantleyCpuDimmTemperatureTelemetryReader::create_context(ipmi::IpmiController& ctrl,
                                                                      TelemetryReader::PtrVector&) {
    return std::make_shared<GrantleyContext>(ctrl);
}

bool GrantleyCpuDimmTemperatureTelemetryReader::is_valid(TelemetryReader::Context::Ptr) const {
    return is_valid_cpu_dimm<NUM_CPUS, NUM_DIMMS, NUM_DIMMS_PER_CPU>();
}


using ipmi::command::sdv::NO_READING;


bool GrantleyCpuTemperatureTelemetryReader::read(Context::Ptr context, ipmi::IpmiController&) {
    GrantleyContext& ctx = static_cast<GrantleyContext&>(*context);
    const auto cpu = get_cpu_and_dimm<NUM_CPUS, NUM_DIMMS, NUM_DIMMS_PER_CPU>().first;
    const auto tjmax = ctx.cpus_thermal_params.get_cpu_tjmax(cpu);
    const auto margin = ctx.cpus_dimms_temp.get_cpu_margin_to_throttle(cpu);
    const auto val = (NO_READING == tjmax) || (NO_READING == margin)
               ? json::Json()
               : json::Json(tjmax - margin);
    return update_value(val);
}


bool GrantleyDimmTemperatureTelemetryReader::read(Context::Ptr context, ipmi::IpmiController&) {
    GrantleyContext& ctx = static_cast<GrantleyContext&>(*context);
    const auto cpu_dimm = get_cpu_and_dimm<NUM_CPUS, NUM_DIMMS, NUM_DIMMS_PER_CPU>();
    const auto dimm_temp = ctx.cpus_dimms_temp.get_dimm_temperature(cpu_dimm.first, cpu_dimm.second);
    const auto val = NO_READING == dimm_temp
                     ? json::Json()
                     : json::Json(dimm_temp);
    return update_value(val);
}


bool GrantleyCpuMarginToThrottleTelemetryReader::read(Context::Ptr context, ipmi::IpmiController&) {
    GrantleyContext& ctx = static_cast<GrantleyContext&>(*context);
    const auto cpu = get_cpu_and_dimm<NUM_CPUS, NUM_DIMMS, NUM_DIMMS_PER_CPU>().first;
    const auto margin_to_throttle = ctx.cpus_dimms_temp.get_cpu_margin_to_throttle(cpu);
    const auto val = NO_READING == margin_to_throttle
                     ? json::Json()
                     : json::Json(margin_to_throttle);
    return update_value(val);
}


}
