/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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


#include "psme/rest-ref/utils/mapper.hpp"
#include "psme/rest-ref/server/utils.hpp"
#include "psme/rest-ref/server/mux/matchers.hpp"
#include "psme/rest-ref/server/error/error_factory.hpp"
#include "psme/rest-ref/constants/constants.hpp"
#include "logger/logger_factory.hpp"

namespace psme {
namespace rest {
namespace model {

using namespace psme::rest::error;
using namespace app::rest::constants;
using namespace psme::rest;

Mapper::RequestParamsMap Mapper::m_params_map = {
    {Root::ETHERNET_SWITCHES, PathParam::ETHERNET_SWITCH_ID},
    {Switch::PORTS, PathParam::SWITCH_PORT_ID},
    {Root::SERVICES, PathParam::SERVICE_ID},
    {StorageService::LOGICAL_DRIVES, PathParam::LOGICAL_DRIVE_ID},
    {SwitchPort::VLANS, PathParam::VLAN_ID}
};

std::string Mapper::get_param_from_component(const std::string& component) {
    if (m_params_map.find(component) != m_params_map.cend()) {
        return m_params_map.at(component);
    }
    const auto& message = "Resource '" + component + "' not recognized.";
    throw ServerException(ErrorFactory::create_not_found_error(message));
}

server::Parameters Mapper::get_params(const std::string& path) {
    server::Parameters params{};
    auto segments = ::psme::rest::server::mux::split_path(path);
    std::string message{};

    if (2 > segments.size() || PathParam::REDFISH != segments[0] || Redfish::V1 != segments[1]) {
        if (path.empty()) {
            message = "URI cannot be empty!";
            auto error = ErrorFactory::create_malformed_uri_error(message);
            throw ServerException(error);
        }
        else {
            message = path + " is invalid.";
            throw ServerException(ErrorFactory::create_not_found_error(message));
        }
    }

    if (segments.size() % 2 != 0) {
        message = "URI should contain collectionName/resourceID pairs.";
        throw ServerException(ErrorFactory::create_not_found_error(message));
    }

    std::regex id_regex{PathParam::ID_REGEX};
    for (std::size_t seg_index = 2 ; seg_index < segments.size(); seg_index++) {
        auto param = get_param_from_component(segments[seg_index]);
        auto param_index = segments[++seg_index];
        if (!std::regex_match(param_index, id_regex)) {
            message = "Resource ID is not a number.";
            throw ServerException(ErrorFactory::create_not_found_error(message));
        }

        params.set(param, param_index);
    }
    return params;
}
}
}
}

