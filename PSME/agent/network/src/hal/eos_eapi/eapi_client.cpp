/*!
 * @brief eAPI Client class implementation.
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
 * @file agent/network/src/hal/eos_eapi/eapi_client.cpp
 */

#include "hal/eos_eapi/eapi_client.hpp"
#include "hal/eos_eapi/get_switch_running_configuration.hpp"
#include "hal/eos_eapi/show_priority_flow_control.hpp"
#include "hal/eos_eapi/show_priority_flow_control_parser.hpp"
#include "logger/logger.hpp"
#include "agent-framework/exceptions/exception.hpp"


using namespace agent::network::hal;

EapiClient::~EapiClient() {}

// TODO: ipv4 address and port should be read from configuration file
EapiClient::EapiClient() :
        m_connector{new json_rpc::HttpClientConnector(make_connection_url("localhost", 8080))},
        m_invoker{new json_rpc::JsonRpcRequestInvoker()}  {}

void EapiClient::start() {
    if (!m_started) {
        m_started = true;
    }
}

void EapiClient::stop() {
    if (m_started) {
        m_started = false;
    }
}

json::Json EapiClient::eapi_request(const EapiCommand& command) const {
    json::Json eapi_json_request = json::Json();
    eapi_json_request["version"] = "latest";
    for(const auto& cmd : command.serialize()) {
        eapi_json_request["cmds"].push_back(cmd);
    }

    eapi_json_request["format"] = command.get_format();
    eapi_json_request["timestamps"] = false;
    log_debug("eapi-client", eapi_json_request.dump());

    m_invoker->prepare_method("runCmds", eapi_json_request);
    m_invoker->call(m_connector);

    json::Json result = m_invoker->get_result();
    log_debug("eapi-client", result.dump());

    if (!result.is_array()) {
        THROW(agent_framework::exceptions::NetworkError, "eapi-client", "Command result is not an array.");
    }

    return result;
}

std::string EapiClient::make_connection_url(const std::string& ipv4address, const int port) const {
    return "http://" + ipv4address + ":" + std::to_string(port) + "/command-api";
}

json::Json EapiClient::get_running_config() const {
    if (m_started) {
        GetSwitchRunningConfiguration command{};
        return eapi_request(command);
    }
    THROW(agent_framework::exceptions::NetworkError, "eapi-client", "Eapi client is not started.");
}

bool EapiClient::is_switch_pfc_enabled() {
    if (m_started) {
        ShowPriorityFlowControl command{};
        const auto eapi_response = eapi_request(command);
        eapi::ShowPriorityFlowControlParser command_parser(eapi_response.dump());
        return command_parser.is_pfc_enabled();
    }
    THROW(agent_framework::exceptions::NetworkError, "eapi-client", "Eapi client is not started.");
}
