/*!
 * @brief Implementation of temperature readers for Purley platform
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
 * @file purley_telemetry_reader.cpp
 */

#include "telemetry/purley_cpu_dimm_temperature_telemetry_reader.hpp"
#include "ipmi/telemetry/get_cpu_and_dimm_temperature.hpp"
#include "ipmi/telemetry/get_cpu_thermal_params.hpp"

namespace {

using MEMORY_MASK = ipmi::command::sdv::request::MEMORY_MASK;
using CHANNEL_SET = ipmi::command::sdv::request::CHANNEL_SET;
constexpr const std::uint16_t DIMMS_MASK_FOR_CH0_TO_3 = MEMORY_MASK::CHANNEL0_DIMM0
                                                        | MEMORY_MASK::CHANNEL1_DIMM0
                                                        | MEMORY_MASK::CHANNEL2_DIMM0
                                                        | MEMORY_MASK::CHANNEL3_DIMM0;
constexpr const std::uint16_t DIMMS_MASK_FOR_CH4_TO_5 = MEMORY_MASK::CHANNEL0_DIMM0
                                                        | MEMORY_MASK::CHANNEL1_DIMM0;
using CpuDimmTempOnCh0_to_3 = ipmi::sdv::GetCpuAndDimmTemperature<DIMMS_MASK_FOR_CH0_TO_3>;
using CpuDimmTempOnCh4_to_5 = ipmi::sdv::GetCpuAndDimmTemperature<DIMMS_MASK_FOR_CH4_TO_5,
                                                                  CHANNEL_SET::CHANNELS_4_TO_7>;
constexpr const auto NUM_CPUS = CpuDimmTempOnCh0_to_3::num_cpus();
constexpr const auto NUM_DIMMS_PER_CPU = CpuDimmTempOnCh0_to_3::num_dimms_per_cpu()
                                         + CpuDimmTempOnCh4_to_5::num_dimms_per_cpu();
constexpr const auto NUM_DIMMS = NUM_DIMMS_PER_CPU * NUM_CPUS;

}

namespace telemetry {

class PurleyContext : public TelemetryReader::Context {
public:
    PurleyContext(ipmi::IpmiController& ctrl) : m_ctrl(ctrl) { }

    ipmi::sdv::CpuThermalParams<NUM_CPUS> cpus_thermal_params{};
    CpuDimmTempOnCh0_to_3 cpus_dimms_on_ch0_to3_temp{};
    CpuDimmTempOnCh4_to_5 cpus_dimms_on_ch4_to5_temp{};

protected:
    bool update() override;

private:
    static const ipmi::BridgeInfo BRIDGE_TO_NM;
    ipmi::IpmiController& m_ctrl;
};

const ipmi::BridgeInfo PurleyContext::BRIDGE_TO_NM = ipmi::BridgeInfo(0x2c, 0x06);

bool PurleyContext::update() {
    cpus_thermal_params.update_readings(m_ctrl, BRIDGE_TO_NM);
    cpus_dimms_on_ch0_to3_temp.update_readings(m_ctrl, BRIDGE_TO_NM);
    cpus_dimms_on_ch4_to5_temp.update_readings(m_ctrl, BRIDGE_TO_NM);
    return true;
}


TelemetryReader::ReaderId PurleyCpuDimmTemperatureTelemetryReader::assigned_reader_id() {
    static ReaderId reader_id = TelemetryReader::assign_reader_id();
    return reader_id;
}

TelemetryReader::Context::Ptr PurleyCpuDimmTemperatureTelemetryReader::create_context(ipmi::IpmiController& ipmi,
                                                                      TelemetryReader::PtrVector&) {
    return std::make_shared<PurleyContext>(ipmi);
}

bool PurleyCpuDimmTemperatureTelemetryReader::is_valid(TelemetryReader::Context::Ptr) const {
    return is_valid_cpu_dimm<NUM_CPUS, NUM_DIMMS, NUM_DIMMS_PER_CPU>();
}


using ipmi::command::sdv::NO_READING;


bool PurleyCpuTemperatureTelemetryReader::read(Context::Ptr context, ipmi::IpmiController&) {
    PurleyContext& ctx = static_cast<PurleyContext&>(*context);
    const int cpu = get_cpu_and_dimm<NUM_CPUS, NUM_DIMMS, NUM_DIMMS_PER_CPU>().first;
    const auto tjmax = ctx.cpus_thermal_params.get_cpu_tjmax(cpu);
    const auto margin = ctx.cpus_dimms_on_ch0_to3_temp.get_cpu_margin_to_throttle(cpu);
    const auto value = (NO_READING == tjmax) || (NO_READING == margin)
                       ? json::Json()
                       : json::Json(tjmax - margin);
    return update_value(value);
}


bool PurleyDimmTemperatureTelemetryReader::read(Context::Ptr context, ipmi::IpmiController&) {
    PurleyContext& ctx = static_cast<PurleyContext&>(*context);
    const auto cpu_dimm = get_cpu_and_dimm<NUM_CPUS, NUM_DIMMS, NUM_DIMMS_PER_CPU>();
    const auto value = (cpu_dimm.second < CpuDimmTempOnCh0_to_3::num_dimms_per_cpu())
                       ? ctx.cpus_dimms_on_ch0_to3_temp.get_dimm_temperature(cpu_dimm.first, cpu_dimm.second)
                       : ctx.cpus_dimms_on_ch4_to5_temp.get_dimm_temperature(cpu_dimm.first,
                                     cpu_dimm.second % CpuDimmTempOnCh4_to_5::num_dimms_per_cpu());
    return update_value(value == NO_READING
                         ? json::Json()
                         : json::Json(value));
}


bool PurleyCpuMarginToThrottleTelemetryReader::read(Context::Ptr context, ipmi::IpmiController&) {
    PurleyContext& ctx = static_cast<PurleyContext&>(*context);
    const int cpu = get_cpu_and_dimm<NUM_CPUS, NUM_DIMMS, NUM_DIMMS_PER_CPU>().first;
    const auto margin_to_throttle = ctx.cpus_dimms_on_ch0_to3_temp.get_cpu_margin_to_throttle(cpu);
    return update_value(margin_to_throttle == NO_READING
                         ? json::Json()
                         : json::Json(margin_to_throttle));
}


}
