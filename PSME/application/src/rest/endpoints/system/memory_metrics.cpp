/*!
 * @brief MemoryMetrics endpoint implementation
 *
 * @header{License}
 * @copyright Copyright (c) 2017 Intel Corporation.
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
 * @header{Filesystem}
 * @file memory_metrics.cpp
 */

#include "psme/rest/endpoints/system/memory_metrics.hpp"
#include "psme/rest/utils/status_helpers.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;
using namespace agent_framework::model::enums;

namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#MemoryMetrics.MemoryMetrics";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#MemoryMetrics.v1_1_2.MemoryMetrics";
    r[Common::ID] = json::Value::Type::NIL;
    r[Common::NAME] = "Memory Metrics";
    r[Common::DESCRIPTION] = "Memory Metrics";
    r[MemoryMetrics::BLOCK_SIZE_BYTES] = json::Value::Type::NIL;

    json::Value current_period;
    current_period[MemoryMetrics::BLOCKS_READ] = json::Value::Type::NIL;
    current_period[MemoryMetrics::BLOCKS_WRITTEN] = json::Value::Type::NIL;
    r[MemoryMetrics::CURRENT_PERIOD] = std::move(current_period);

    json::Value life_time;
    life_time[MemoryMetrics::BLOCKS_READ] = json::Value::Type::NIL;
    life_time[MemoryMetrics::BLOCKS_WRITTEN] = json::Value::Type::NIL;
    r[MemoryMetrics::LIFE_TIME] = std::move(life_time);

    json::Value alarm_trips;
    alarm_trips[MemoryMetrics::TEMPERATURE] = json::Value::Type::NIL;
    alarm_trips[MemoryMetrics::SPARE_BLOCK] = json::Value::Type::NIL;
    alarm_trips[MemoryMetrics::UNCORRECTABLE_ECC_ERROR] = json::Value::Type::NIL;
    alarm_trips[MemoryMetrics::CORRECTABLE_ECC_ERROR] = json::Value::Type::NIL;
    alarm_trips[MemoryMetrics::ADDRESS_PARITY_ERROR] = json::Value::Type::NIL;

    json::Value health_data;
    health_data[Common::ODATA_TYPE] = "#MemoryMetrics.v1_1_0.HealthData";
    health_data[MemoryMetrics::REMAINING_SPARE_BLOCK_PERCENTAGE] = json::Value::Type::NIL;
    health_data[MemoryMetrics::LAST_SHUTDOWN_SUCCESS] = json::Value::Type::NIL;
    health_data[MemoryMetrics::DATA_LOSS_DETECTED] = json::Value::Type::NIL;
    health_data[MemoryMetrics::PERFORMANCE_DEGRADED] = json::Value::Type::NIL;
    health_data[MemoryMetrics::PREDICTED_MEDIA_LIFE_LEFT_PERCENT] = json::Value::Type::NIL;
    health_data[MemoryMetrics::ALARM_TRIPS] = std::move(alarm_trips);
    r[MemoryMetrics::HEALTH_DATA] = std::move(health_data);

    json::Value actions;
    actions[Common::OEM] = json::Value::Type::OBJECT;
    r[Common::ACTIONS] = std::move(actions);

    json::Value rackscale;
    rackscale[Common::ODATA_TYPE] = "#Intel.Oem.MemoryMetrics";
    rackscale[MemoryMetrics::TEMPERATURE_CELSIUS] = json::Value::Type::NIL;
    rackscale[MemoryMetrics::THROTTLED_CYCLES_PERCENT] = json::Value::Type::NIL;
    rackscale[MemoryMetrics::BANDWIDTH_PERCENT] = json::Value::Type::NIL;
    rackscale[MemoryMetrics::THERMAL_MARGIN_CELSIUS] = json::Value::Type::NIL;
    rackscale[MemoryMetrics::CONSUMED_POWER_WATT] = json::Value::Type::NIL;
    rackscale[MemoryMetrics::ECC_ERRORS_COUNT] = json::Value::Type::NIL;
    rackscale[MemoryMetrics::HEALTH] = json::Value::Type::ARRAY;
    r[Common::OEM][Common::RACKSCALE] = std::move(rackscale);

    return r;
}

}


endpoint::MemoryMetrics::MemoryMetrics(const std::string& path) : EndpointBase(path) {}


endpoint::MemoryMetrics::~MemoryMetrics() {}


void endpoint::MemoryMetrics::get(const server::Request& req, server::Response& res) {
    using agent_framework::model::System;
    using agent_framework::model::Memory;
    auto memory_uuid = psme::rest::model::Find<Memory>(req.params[PathParam::MEMORY_ID])
                                    .via<System>(req.params[PathParam::SYSTEM_ID]).get_uuid();
    auto json = make_prototype();
    json[Common::ODATA_ID] = PathBuilder(req).build();
    json[Common::ID] = "Memory " + req.params[PathParam::MEMORY_ID] + " Metrics";
    utils::populate_metrics(json, memory_uuid);

    set_response(res, json);
}
