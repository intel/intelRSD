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
 * @file bandwidth_telemetry_reader.cpp
 */

#include "telemetry/bandwidth_telemetry_reader.hpp"

#include "ipmi/telemetry/cpus_bandwidth.hpp"
#include "ipmi/telemetry/get_cups_data.hpp"


namespace telemetry {

class BandwidthContext : public TelemetryReader::Context {
public:
    BandwidthContext(ipmi::IpmiController& ctrl) : m_ctrl(ctrl) { }

    ipmi::sdv::CpusBandwidth cpus_bandwidth{};
    ipmi::sdv::GetCupsData cups_data{};

protected:
    bool update() override;

private:
    static const ipmi::BridgeInfo BRIDGE_TO_NM;
    ipmi::IpmiController& m_ctrl;
};

const ipmi::BridgeInfo BandwidthContext::BRIDGE_TO_NM = ipmi::BridgeInfo(0x2c, 0x06);

bool BandwidthContext::update() {
    cpus_bandwidth.update_readings(m_ctrl, BRIDGE_TO_NM);
    cups_data.update_readings(m_ctrl, BRIDGE_TO_NM);
    return true;
}


TelemetryReader::ReaderId BandwidthTelemetryReader::assigned_reader_id() {
    static ReaderId reader_id = TelemetryReader::assign_reader_id();
    return reader_id;
}

TelemetryReader::Context::Ptr BandwidthTelemetryReader::create_context(ipmi::IpmiController& ctrl,
                                                                      TelemetryReader::PtrVector&) {
    return std::make_shared<BandwidthContext>(ctrl);
}


bool CpuBandwidthTelemetryReader::read(Context::Ptr context, ipmi::IpmiController&) {
    BandwidthContext& ctx = static_cast<BandwidthContext&>(*context);
    const auto value = ctx.cpus_bandwidth.get_bandwidth_percent();
    return update_value(value == ipmi::command::sdv::NO_READING
                         ? json::Json()
                         : json::Json(value));
}


bool MemoryBandwidthTelemetryReader::read(Context::Ptr context, ipmi::IpmiController&) {
    BandwidthContext& ctx = static_cast<BandwidthContext&>(*context);
    const auto value = ctx.cups_data.get_memory_bandwidth_percent();
    return update_value(value == ipmi::command::sdv::NO_READING
                         ? json::Json()
                         : json::Json(value));
}


bool IoBandwidthTelemetryReader::read(Context::Ptr context, ipmi::IpmiController&) {
    BandwidthContext& ctx = static_cast<BandwidthContext&>(*context);
    const auto value = ctx.cups_data.get_io_bandwidth_percent();
    return update_value(value == ipmi::command::sdv::NO_READING
                         ? json::Json()
                         : json::Json(value));
}

}
