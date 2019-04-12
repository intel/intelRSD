/*!
 * @brief Implementation of TelemetryHub.
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
 * @file telemetry/telemetry_hub.cpp
 */

#include "ipmi/telemetry/telemetry_hub.hpp"
#include "ipmi/command/sdv/enums.hpp"
#include "ipmi/command/sdv/nm/query_reading_package.hpp"
#include "ipmi/command/sdv/nm/update_reading_package.hpp"
#include "ipmi/command/sdv/nm/get_telemetry_readings.hpp"
#include "ipmi/command/sdv/nm/errors.hpp"

using namespace ipmi::sdv;
using namespace ipmi::command::sdv;

namespace {

/*
 *  Valid package guids [1, 9)
 */
constexpr std::pair<std::uint32_t, std::uint32_t> PACKAGE_GUID_RANGE = std::make_pair(1, 9);

}


MetricIdVec TelemetryHub::get_package_metrics(std::uint32_t package_guid) const {
    std::uint8_t start_reading_index{0};
    request::QueryReadingPackage query_package_req{};
    response::QueryReadingPackage query_package_resp{};
    query_package_req.set_package_guid(package_guid);
    query_package_req.set_start_reading_index(start_reading_index);
    m_ipmi.send(query_package_req, m_bridge, query_package_resp);
    return query_package_resp.get_metric_ids();
}


Readings TelemetryHub::get_package_readings(std::uint32_t package_guid) const {
    auto metric_ids = get_package_metrics(package_guid);
    std::uint8_t start_reading_index{0};
    request::GetTelemetryReadings get_readings_req{};
    response::GetTelemetryReadings get_readings_resp{};
    get_readings_req.set_package_guid(package_guid);
    get_readings_req.set_start_reading_index(start_reading_index);
    m_ipmi.send(get_readings_req, m_bridge, get_readings_resp);
    auto raw_readings = get_readings_resp.get_readings();

    if (raw_readings.size() != metric_ids.size()) {
        throw ipmi::ResponseLengthError(get_readings_resp, metric_ids.size(), raw_readings.size());
    }

    Readings readings{};
    std::size_t idx = 0;
    for (const auto& reading_id: metric_ids) {
        readings.emplace_back(Reading{reading_id, raw_readings[idx++]});
    }

    return readings;
}

std::uint32_t TelemetryHub::add_metrics_to_package(std::uint32_t package_guid,
                                                   const MetricIdVec& metric_ids) const {
    request::UpdateReadingPackage update_package_req{};
    response::UpdateReadingPackage update_package_resp{};
    update_package_req.set_package_guid(package_guid);
    update_package_req.set_metric_ids(metric_ids);
    m_ipmi.send(update_package_req, m_bridge, update_package_resp);
    return update_package_resp.get_reading_package_guid();
}


std::uint32_t TelemetryHub::create_package(const MetricIdVec& readings) const {
    return add_metrics_to_package(0, readings);
}

void TelemetryHub::remove_package(std::uint32_t package_guid) const {
    request::UpdateReadingPackage update_package_req{};
    response::UpdateReadingPackage update_package_resp{};
    update_package_req.set_package_guid(package_guid);
    update_package_req.delete_reading_package();
    m_ipmi.send(update_package_req, m_bridge, update_package_resp);
}

TelemetryHub::PackagesWithReadings TelemetryHub::load_packages() const {
    PackagesWithReadings packages{};
    for (auto guid = PACKAGE_GUID_RANGE.first; guid < PACKAGE_GUID_RANGE.second; ++guid) {
        try {
            Readings readings{};
            auto metric_ids = get_package_metrics(guid);
            for (const auto& metric : metric_ids) {
                readings.emplace_back(metric);
            }
            packages.emplace(guid, std::move(readings));
        }
        catch (const ipmi::command::sdv::InvalidPackageGuid&) {
            // omit non existing packages
        }
    }
    return packages;
}

TelemetryHub::PackagesWithReadings TelemetryHub::load_packages_with_readings() const {
    PackagesWithReadings packages_with_readings{};
    for (auto guid = PACKAGE_GUID_RANGE.first; guid < PACKAGE_GUID_RANGE.second; ++guid) {
        try {
            packages_with_readings.emplace(guid, get_package_readings(guid));
        }
        catch (const ipmi::command::sdv::InvalidPackageGuid&) {
            // omit non existing packages
        }
    }
    return packages_with_readings;
}
