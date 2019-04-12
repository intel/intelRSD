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
 * @file rmm_cert_loader.cpp
 *
 * */

#include "psme/rest/server/certs/rmm_cert_loader.hpp"
#include "psme/core/agent/agent_manager.hpp"

#include "agent-framework/module/requests/chassis/get_authorization_certificate.hpp"
#include "agent-framework/module/model/authorization_certificate.hpp"

#include "configuration/configuration.hpp"
#include "logger/logger_factory.hpp"
#include "base64/base64.hpp"

#include <chrono>
#include <thread>

using namespace psme::rest::server;

RMMCertLoader::RMMCertLoader(const std::string& certs_dir)
    : CertLoader{certs_dir} { }

namespace {
    std::string get_cert_from_agent(psme::core::agent::JsonAgent& agent) {
        using agent_framework::model::requests::GetAuthorizationCertificate;
        GetAuthorizationCertificate request{"PODM"};
        auto auth_cert = agent.execute<agent_framework::model::AuthorizationCertificate>(request);
        const auto cert = base64::Base64::decode(auth_cert.get_certificate());
        std::string cert_string(cert.cbegin(), cert.cend());
        log_info("rest", "CA certificate loaded.");
        log_debug("rest", "CA certificate:\n" << cert_string);
        return cert_string;
    }
}

std::string RMMCertLoader::load_ca_cert() {
    using core::agent::AgentManager;
    const std::chrono::seconds WAIT_TIME{2};
    while (true) {
        log_info("rest", "Waiting for CA certificate...");
        auto chassis_agents =
            AgentManager::get_instance()->get_agents_by_capability("Chassis");

        for (const auto& chassis_agent: chassis_agents) {
            try {
                return get_cert_from_agent(*chassis_agent);
            } catch (const std::exception& e) {
                log_error("rest",
                        "Failed to retrieve certificate: " << e.what());
            } catch (...) {
                log_error("rest",
                        "Failed to retrieve certificate: Unknown exception.");
            }
        }
        std::this_thread::sleep_for(WAIT_TIME);
    }
}
