/*!
 * @brief Implementation of Intel Optane DC Persistent Memory telemetry reader
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
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
 * @file dcpmem_telemetry_reader.cpp
 */

#include "telemetry/dcpmem_telemetry_reader.hpp"
#include "ipmi/telemetry/dcpmem_telemetry_data.hpp"
#include "utils/conversion.hpp"

namespace telemetry {

class DcpmemContext : public TelemetryReader::Context {
public:
    DcpmemContext(ipmi::IpmiController& ctrl) : m_ipmi_ctrl(ctrl) { }

    ipmi::sdv::DcpmemTelemetryData dcpmem_telemetry_data{};

protected:
    bool update() override;

private:
    static const ipmi::BridgeInfo m_nm_bridge;
    ipmi::IpmiController& m_ipmi_ctrl;
};

static constexpr const uint32_t SPS_NM_ADDRESS = 0x2c;
static constexpr const uint8_t SPS_NM_CHANNEL = 0x06;

const ipmi::BridgeInfo DcpmemContext::m_nm_bridge = ipmi::BridgeInfo(SPS_NM_ADDRESS, SPS_NM_CHANNEL);

bool DcpmemContext::update() {
    dcpmem_telemetry_data.update_readings(m_ipmi_ctrl, m_nm_bridge);
    return true;
}

TelemetryReader::ReaderId DcpmemTelemetryReader::assigned_reader_id() {
    static ReaderId reader_id = TelemetryReader::assign_reader_id();
    return reader_id;
}

TelemetryReader::Context::Ptr DcpmemTelemetryReader::create_context(ipmi::IpmiController& ipmi, TelemetryReader::PtrVector&) {
    auto ctx = std::make_shared<DcpmemContext>(ipmi);

    // Initialize DcpmemDimmLocation objects container on DcpmemContext creation
    auto dcpmem_locations = pmem::MemoryTopology::get_dcpmem_locations_for_memory_mapping(pmem::MemoryTopology::get_memory_location_mapping());
    ctx->dcpmem_telemetry_data.set_dcpmem_dimm_locations(dcpmem_locations);

    return ctx;
}

bool DcpmemTelemetryReader::is_valid(TelemetryReader::Context::Ptr) const {
    const auto memory_instance = get_resource_key().get_instance_number(); // 0-index based Memory instance number

    return (ResourceInstance::Component::Memory == get_resource_key().get_component() &&
           (memory_instance >= 0 && memory_instance < pmem::MemoryTopology::get_number_of_supported_dimms()));
}

bool DcpmemTelemetryReader::read(Context::Ptr context, ipmi::IpmiController&) {
    DcpmemContext& ctx = static_cast<DcpmemContext&>(*context);

    const auto memory_instance = get_resource_key().get_instance_number();

    const auto& smart_info = ctx.dcpmem_telemetry_data.get_smart_info(memory_instance);
    if (smart_info.ValidationFlags == 0) {
        return false; // Invalid readings - exit
    }

    const auto& memory_info = ctx.dcpmem_telemetry_data.get_memory_info(memory_instance);

    log_debug("dcpmem-telemetry", "Updated Telemetry readings: DCPMEM ResourceInstance[" << memory_instance << "]");
    return update_value(process_telemetry_reading(get_metric_definition(), smart_info, memory_info));
}

json::Json DcpmemTelemetryReader::process_telemetry_reading(const MetricDefinition& metric,
                                                            const pmem::structs::SMART_INFO& smart_info,
                                                            const pmem::structs::MEMORY_INFO& memory_info) {
    json::Json metric_json{};
    MemoryMetricName memory_metric{MemoryMetricName::memoryMetricUnsupported};
    try {
        memory_metric = MemoryMetricName::from_string(metric.get_name());
    }
    catch (agent_framework::exceptions::InvalidValue&) {
        log_warning("dcpmem-telemetry", "Incorrect Memory Metric: " << metric.get_name());
        return metric_json;
    }

    switch (memory_metric) {
        case MemoryMetricName::memoryLastShutdownSuccess:
            metric_json = json::Json(smart_info.LastShutdownStatus == SMART_INFO_LAST_SHUTDOWN_STATUS_CLEAN);
            break;
        case MemoryMetricName::memoryPredictedMediaLifeLeftPercent:
            metric_json = json::Json(smart_info.PercentageRemaining);
            break;
        case MemoryMetricName::memoryAlarmTripsTemperature:
            metric_json = json::Json(smart_info.AlarmTrips & SMART_INFO_ALARM_TRIPS_MEDIA_TEMPERATURE ||
                                     smart_info.AlarmTrips & SMART_INFO_ALARM_TRIPS_CORE_TEMPERATURE);
            break;
        case MemoryMetricName::memoryControllerTemperature: {
            metric_json = json::Json(SMART_INFO_TEMPERATURE(smart_info.ControllerTemperature));
            break;
        }
        case MemoryMetricName::memoryUptimeSeconds: {
            metric_json = json::Json(smart_info.Uptime);
            break;
        }
        case MemoryMetricName::memoryUnsafeShutdownCount: {
            metric_json = json::Json(smart_info.DirtyShutdownCount);
            break;
        }
        case MemoryMetricName::memoryPowerCycles: {
            metric_json = json::Json(smart_info.PowerCycles);
            break;
        }
        case MemoryMetricName::memoryPowerOnTimeSeconds: {
            metric_json = json::Json(smart_info.PowerOnTime);
            break;
        }
        case MemoryMetricName::memoryCurrentPeriodBlocksRead: {
            const auto bytes_read = utils::buffer_to_integer(memory_info.MediaReads, sizeof(memory_info.MediaReads));
            metric_json = json::Json(bytes_read / m_block_size);
            break;
        }
        case MemoryMetricName::memoryCurrentPeriodBlocksWritten: {
            const auto bytes_written = utils::buffer_to_integer(memory_info.MediaWrites, sizeof(memory_info.MediaWrites));
            metric_json = json::Json(bytes_written / m_block_size);
            break;
        }
        case MemoryMetricName::memoryCurrentPeriodHostReadRequests: {
            metric_json = json::Json(utils::buffer_to_integer(memory_info.ReadRequests, sizeof(memory_info.ReadRequests)));
            break;
        }
        case MemoryMetricName::memoryCurrentPeriodHostWriteRequests: {
            metric_json = json::Json(utils::buffer_to_integer(memory_info.WriteRequests, sizeof(memory_info.WriteRequests)));
            break;
        }
        case MemoryMetricName::memoryMetricUnsupported:
        default:
            log_warning("dcpmem-telemetry", "Unsupported Memory Metric");
    }

    log_debug("dcpmem-telemetry", "-> Metric: " << memory_metric.to_string() << " = " << metric_json);
    return metric_json;
}

}