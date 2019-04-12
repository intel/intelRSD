/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 *
 * */

#include "psme/ssdp/ssdp_config_loader.hpp"
#include "psme/rest/server/connector/connector_options_loader.hpp"
#include "psme/rest/constants/constants.hpp"
#include "agent-framework/module/service_uuid.hpp"
#include "json-wrapper/json-wrapper.hpp"

namespace {

std::string get_service_url(const json::Json& config) {
    std::string service_url{};
    for (const auto& conn_options: psme::rest::server::load_connectors_options(config)) {
        if (conn_options.use_ssl()) {
            service_url += "https://localhost:"
                    + std::to_string(int(conn_options.get_port()))
                    + psme::rest::constants::PathParam::BASE_URL;
            break;
        }
    }
    return service_url;
}

}

namespace ssdp {

SsdpServiceConfig load_ssdp_config(const json::Json& config, const std::string& uuid) {
    using agent_framework::module::ServiceUuid;
    using std::chrono::seconds;
    SsdpServiceConfig ssdp_service_config;

    const auto& ssdp_config = config.value("ssdp-service", json::Json::object());
    if (ssdp_config.value("enabled", bool{})) {
        ssdp_service_config.enable_ssdp_service();
    }
    auto announce_interval = seconds(ssdp_config.value("announce-interval-seconds", uint16_t{}));
    ssdp_service_config.set_announce_interval(announce_interval);
    if (config.count("server") && config["server"].count("network-interface-name")) {
        for (const auto& nic_name: config["server"]["network-interface-name"]) {
            ssdp_service_config.add_nic_name(nic_name.get<std::string>());
        }
    }
    ssdp_service_config.set_socket_ttl(ssdp_config.value("ttl", std::uint8_t{}));
    ssdp_service_config.set_service_uuid(uuid);
    ssdp_service_config.set_service_urn("urn:dmtf-org:service:redfish-rest:1");
    ssdp_service_config.set_service_url(get_service_url(config));

    return ssdp_service_config;
}

}
