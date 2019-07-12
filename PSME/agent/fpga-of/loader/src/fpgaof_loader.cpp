/*!
 * @brief Load FPGA-oF agent configuration.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License") override;
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License
 *
 * @file fpgaof_loader.cpp
 */

#include "loader/fpgaof_loader.hpp"
#include "logger/logger.hpp"
#include "utils.hpp"



using namespace agent::fpgaof::loader;

namespace {

void check_required_fields(const json::Json& config) {
    if (!config.count("agent")) {
        throw std::runtime_error("'agent' field is required.");
    }
    const auto& agent_prop = config["agent"];
    if (!agent_prop.is_object()) {
        throw std::runtime_error("'agent' field should be an object");
    }

    if (!agent_prop.count("vendor") ||
        !agent_prop["vendor"].is_string()) {
        throw std::runtime_error("'agent:vendor' field is required and should be a string.");
    }

    if (!agent_prop.count("capabilities") ||
        !agent_prop["capabilities"].is_array()) {
        throw std::runtime_error("'agent:capabilities' field is required and should be an array.");
    }

    if (!config.count("registration")) {
        throw std::runtime_error("'registration' field is required.");
    }
    const auto& registration_prop = config["registration"];
    if (!registration_prop.is_object()) {
        throw std::runtime_error("'registration' field should be an object");
    }

    if (!registration_prop.count("ipv4") ||
        !registration_prop["ipv4"].is_string()) {
        throw std::runtime_error("'registration:ipv4' field is required and should be a string.");
    }

    if (!registration_prop.count("port") ||
        !registration_prop["port"].is_number()) {
        throw std::runtime_error("'registration:port' field is required and should be a number.");
    }

    if (!registration_prop.count("interval") ||
        !registration_prop["interval"].is_number()) {
        throw std::runtime_error("'registration:interval' field is required and should be a number.");
    }

    if (!config.count("opae-proxy")) {
        throw std::runtime_error("'opae-proxy' field is required.");
    }
    const auto& opae_proxy_prop = config["opae-proxy"];
    if (!opae_proxy_prop.is_object()) {
        throw std::runtime_error("'opae-proxy' field should be an object");
    }

	if (!opae_proxy_prop.count("transports") ||
        !opae_proxy_prop["transports"].is_array()) {
		throw std::runtime_error("'opae-proxy:transports' field is required and should be an array.");
	}

	if(opae_proxy_prop["transports"].empty()) {
        throw std::runtime_error("Array 'opae-proxy:transports' should have at least one entry.");
	}

	for (const auto& transport: opae_proxy_prop["transports"]) {
		if (!transport.is_object()) {
			throw std::runtime_error("Each entry in 'opae-proxy:transports' should be an object.");
		}
		if (!transport.count("ipv4") ||
			!transport["ipv4"].is_string()) {
			throw std::runtime_error("'opae-proxy:transports:ipv4' field is required and should be a string.");
		}

		if (!transport.count("port") ||
			!transport["port"].is_number()) {
			throw std::runtime_error("'opae-proxy:transports:port' field is required and should be a number.");
		}

		if (!transport.count("protocol") ||
			!transport["protocol"].is_string()) {
			throw std::runtime_error("'opae-proxy:transports:protocol' field is required and should be a string.");
		}

		std::string transport_protocol = transport["protocol"].get<std::string>();
		if (transport_protocol != "TCP" && transport_protocol != "RDMA") {
			throw std::runtime_error("Allowable values in each entry in 'opae-proxy:transports:protocol' are 'TCP' or 'RDMA'.");
		}

		if (transport_protocol == "RDMA") {
		    bool rdma_supported = agent::fpgaof::utils::is_rdma_supported_for_address(transport["ipv4"]);
		    if (!rdma_supported) {
                throw std::runtime_error("'opae-proxy:transports:protocol' field has 'RDMA' value, but the interface with IP address '" + transport["ipv4"].get<std::string>() + "' doesn't support RDMA.");
		    }
		}
	}

	if (!opae_proxy_prop.count("secureEraseGBS") ||
	    !opae_proxy_prop["secureEraseGBS"].is_string()) {
		throw std::runtime_error("'opae-proxy:secureEraseGBS' field should be a string");
	}

    if (!config.count("server")) {
        throw std::runtime_error("'server' field is required.");
    }
    const auto& server_prop = config["server"];
    if (!server_prop.is_object()) {
        throw std::runtime_error("'server' field should be an object");
    }

    if (!server_prop.count("port") ||
        !server_prop["port"].is_number()) {
        throw std::runtime_error("'server:port' field is required and should be a number.");
    }

    if (!config.count("managers") || !config["managers"].is_array()) {
        throw std::runtime_error("'managers' field is required and should be an array.");
    }
    auto& managers_array = config["managers"];

    if (1 != managers_array.size()) {
        throw std::runtime_error("'managers' array should have exactly one entry.");
    }

    for(const auto& manager: managers_array) {
        if (!manager.count("ipv4") || !manager["ipv4"].is_string()) {
            throw std::runtime_error("Each entry in manager must have 'ipv4' field.");
        }
        if (!manager.count("locationOffset") || !manager["locationOffset"].is_number()) {
            throw std::runtime_error("Each entry in manager must have "
                                         "'locationOffset' field.");
        }
        if (!manager.count("parentId") || !manager["parentId"].is_string()) {
            throw std::runtime_error("Each entry in manager must have "
                                         "'parentId' field.");
        }
    }
}


void load_manager(FpgaofConfiguration& configuration, const json::Json& manager_json) {
    configuration.set_ipv4_address(manager_json["ipv4"].get<std::string>());
    std::vector<std::string> drivers{};
    configuration.set_location_offset(manager_json["locationOffset"].get<std::uint32_t>());
    configuration.set_parent_id(manager_json["parentId"].get<std::string>());
}

void load_managers(FpgaofConfiguration& configuration, const json::Json& config) {
    auto& managers_array = config["managers"];

    for (const auto& manager_json : managers_array) {
        load_manager(configuration, manager_json);
        // not expecting more managers then one for our agent
        break;
    }
}

void load_opae_proxy_transports(FpgaofConfiguration& configuration, const json::Json& config_json) {
	std::vector<opaepp::TransportDetails> transports;
	auto& transports_json_array = config_json["transports"];

	for (const auto& transport_object_json: transports_json_array) {
		std::string protocol_name = transport_object_json["protocol"].get<std::string>();
		std::string ipv4_address = transport_object_json["ipv4"].get<std::string>();
		uint16_t port = transport_object_json["port"].get<uint16_t>();
		opaepp::TransportDetails transport_details(protocol_name, ipv4_address, port);
		transports.push_back(transport_details);
	}

	configuration.set_opae_proxy_transports(transports);
}

void load_opae_proxy(FpgaofConfiguration& configuration, const json::Json& config_json) {
    auto& opae_proxy_obj = config_json["opae-proxy"];
    load_opae_proxy_transports(configuration, opae_proxy_obj);

	auto& secure_erase_gbs_obj = opae_proxy_obj["secureEraseGBS"];
	configuration.set_secure_erase_gbs_file_path(secure_erase_gbs_obj.get<std::string>());
}

}


FpgaofLoader::FpgaofLoader() : m_configuration(std::make_shared<FpgaofConfiguration>()) {
}


bool FpgaofLoader::load(const json::Json& json) {
    try {
        check_required_fields(json);
        load_managers(*m_configuration, json);
        load_opae_proxy(*m_configuration, json);
    }
    catch (const std::exception& e) {
        log_error("fpgaof-agent", "Load module configuration failed: " << e.what());
        return false;
    }
    return true;
}


std::shared_ptr<FpgaofConfiguration> FpgaofLoader::get() const {
    return m_configuration;
}

