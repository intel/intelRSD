/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * */


#include "psme/rest/utils/mapper.hpp"
#include "psme/rest/server/utils.hpp"
#include "psme/rest/server/mux/matchers.hpp"
#include "psme/rest/server/error/error_factory.hpp"
#include "psme/rest/constants/constants.hpp"
#include "agent-framework/exceptions/exception.hpp"

using namespace psme::rest;
using namespace psme::rest::constants;
using namespace psme::rest::server;
using namespace psme::rest::error;

namespace {

void check_path(const std::string& path_template, const std::string& path) {
    auto request_segments = mux::split_path(path);
    auto endpoint_segments = mux::path_to_segments(path_template);

    auto match = mux::segments_match(endpoint_segments, request_segments);
    if (!match) {
        THROW(agent_framework::exceptions::NotFound, "rest",
              path + " do not match requested resource.");
    }
}

}

namespace psme {
namespace rest {
namespace model {

Mapper::RequestParamsMap Mapper::m_params_map = {
    {Root::ETHERNET_SWITCHES, PathParam::ETHERNET_SWITCH_ID},
    {EthernetSwitch::PORTS, PathParam::SWITCH_PORT_ID},
    {Root::SERVICES, PathParam::SERVICE_ID},
    {Common::CHASSIS, PathParam::CHASSIS_ID},
    {Chassis::DRIVES, PathParam::DRIVE_ID},
    {StorageService::LOGICAL_DRIVES, PathParam::LOGICAL_DRIVE_ID},
    {EthernetSwitchPort::VLANS, PathParam::VLAN_ID},
    {Chassis::PCIE_DEVICES, PathParam::DEVICE_ID},
    {PathParam::FUNCTIONS, PathParam::FUNCTION_ID},
    {Root::FABRICS, PathParam::FABRIC_ID},
    {Fabric::SWITCHES, PathParam::SWITCH_ID},
    {Fabric::ENDPOINTS, PathParam::ENDPOINT_ID},
    {Fabric::ZONES, PathParam::ZONE_ID},
    {Switch::PORTS, PathParam::PORT_ID}
};

std::string Mapper::get_param_from_component(const std::string& component) {
    if (m_params_map.find(component) != m_params_map.cend()) {
        return m_params_map.at(component);
    }
    const auto& message = "Resource '" + component + "' is not recognized.";
    throw agent_framework::exceptions::NotFound(message);
}

server::Parameters Mapper::get_params(const std::string& path, const std::string& path_template) {
    server::Parameters params{};
    const auto segments = ::psme::rest::server::mux::split_path(path);
    std::string message{};

    if (segments.empty()) {
        message = "URI cannot be empty!";
        throw agent_framework::exceptions::NotFound(message);
    }

    if ((segments.size() > 0 && PathParam::REDFISH != segments[0]) ||
        (segments.size() > 1 && Redfish::V1 != segments[1])) {
        message = path + " is not in /redfish/v1 namespace.";
        throw agent_framework::exceptions::NotFound(message);
    }

    if (segments.size() % 2 != 0) {
        message = "URI should end with collectionName/resourceID pairs.";
        throw agent_framework::exceptions::NotFound(message);
    }

    std::regex id_regex{PathParam::ID_REGEX};
    for (std::size_t seg_index = 2; seg_index < segments.size(); seg_index++) {
        auto param = get_param_from_component(segments[seg_index]);
        auto param_index = segments[++seg_index];
        if (!std::regex_match(param_index, id_regex)) {
            message = "Resource ID is not a number.";
            throw agent_framework::exceptions::NotFound(message);
        }

        params.set(param, param_index);
    }

    // Check if URI is correct against resource path schema
    check_path(path_template, path);

    return params;
}
}
}
}

