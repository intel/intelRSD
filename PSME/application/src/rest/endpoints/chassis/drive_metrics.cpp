/*!
 * @brief DriveMetrics endpoint implementation
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
 * @file drive_metrics.cpp
 */

#include "psme/rest/endpoints/chassis/drive_metrics.hpp"
#include "psme/rest/endpoints/utils.hpp"

using namespace psme::rest;
using namespace psme::rest::constants;
using namespace agent_framework::model::enums;

namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#DriveMetrics.DriveMetrics";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#DriveMetrics.v1_0_0.DriveMetrics";
    r[Common::ID] = json::Json::value_t::null;
    r[Common::NAME] = "Drive Metrics";
    r[Common::DESCRIPTION] = "DriveMetrics contains usage and health statistics for a single physical Drive.";
    r[DriveMetrics::TEMPERATURE_KELVIN] = json::Json::value_t::null;

    json::Json lifetime = json::Json();
    lifetime[Common::ODATA_TYPE] = "#DriveMetrics.v1_0_0.LifeTime";
    lifetime[DriveMetrics::UNIT_SIZE_BYTES] = json::Json::value_t::null;
    lifetime[DriveMetrics::UNITS_READ] = json::Json::value_t::null;
    lifetime[DriveMetrics::UNITS_WRITTEN] = json::Json::value_t::null;
    lifetime[DriveMetrics::HOST_READ_COMMANDS] = json::Json::value_t::null;
    lifetime[DriveMetrics::HOST_WRITE_COMMANDS] = json::Json::value_t::null;
    lifetime[DriveMetrics::POWER_CYCLES] = json::Json::value_t::null;
    lifetime[DriveMetrics::POWER_ON_HOURS] = json::Json::value_t::null;
    lifetime[DriveMetrics::CONTROLLER_BUSY_TIME_MINUTES] = json::Json::value_t::null;
    r[DriveMetrics::LIFETIME] = std::move(lifetime);

    json::Json health_data = json::Json();
    health_data[Common::ODATA_TYPE] = "#DriveMetrics.v1_0_0.HealthData";
    health_data[DriveMetrics::AVAILABLE_SPARE_PERCENTAGE] = json::Json::value_t::null;
    health_data[DriveMetrics::PREDICTED_MEDIA_LIFE_USED_PERCENT] = json::Json::value_t::null;
    health_data[DriveMetrics::UNSAFE_SHUTDOWNS] = json::Json::value_t::null;
    health_data[DriveMetrics::MEDIA_ERRORS] = json::Json::value_t::null;
    r[DriveMetrics::HEALTH_DATA] = std::move(health_data);

    return r;
}

}


endpoint::DriveMetrics::DriveMetrics(const std::string& path) : EndpointBase(path) {}


endpoint::DriveMetrics::~DriveMetrics() {}


void endpoint::DriveMetrics::get(const server::Request& req, server::Response& res) {

    auto drive_uuid = model::find<agent_framework::model::Chassis, agent_framework::model::Drive>(
        req.params).get_uuid();

    auto json = make_prototype();
    json[Common::ODATA_ID] = PathBuilder(req).build();
    json[Common::ID] = constants::Common::METRICS;
    json[Common::NAME] = "Drive " + req.params[PathParam::DRIVE_ID] + " Metrics";

    utils::populate_metrics(json, drive_uuid);

    set_response(res, json);
}
