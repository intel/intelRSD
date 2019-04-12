/*!
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
 * @file psme/rest/server/connector/connector_options_loader.cpp
 */

#include "psme/rest/server/connector/connector_options_loader.hpp"



namespace psme {
namespace rest {
namespace server {

ConnectorOptionsVec load_connectors_options(const json::Json& config) {
    ConnectorOptionsVec connector_options{};
    if (config.count("server") &&
        config["server"].count("connectors") &&
        config["server"].count("network-interface-name")) {
        const auto& connectors_config = config["server"]["connectors"];
        const auto& network_interface_names = config["server"]["network-interface-name"];
        for (const auto& iface_name: network_interface_names) {
            for (const auto& connector_config : connectors_config) {
                connector_options.emplace_back(connector_config, iface_name.get<std::string>());
            }
        }
    }
    if (connector_options.empty()) {
        throw std::runtime_error("No valid connector configuration found.");
    }
    return connector_options;
}

}
}
}
