/*!
 * @header{License}
 * @copyright Copyright (c) 2017-2018 Intel Corporation.
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
 * @file rmm/loader/rmm_loader.cpp
 */

#include "loader/rmm_loader.hpp"

#include "configuration/configuration.hpp"
#include "logger/logger_factory.hpp"
#include "agent-framework/version.hpp"
#include "agent-framework/service_uuid.hpp"
#include "certificate_management/certificate_manager.hpp"
#include "loader/ipmi_config.hpp"
#include "ipmi/manager/ipmitool/serial_controller.hpp"


using namespace agent::rmm::loader;
using namespace agent::rmm;

namespace {

void check_required_fields(const json::Value& config) {
    if (!config["service"].is_string()) {
        throw std::runtime_error("'service' field is required.");
    }

    if (!config["agent"].is_object()) {
        throw std::runtime_error("'agent' field is required.");
    }

    if (!config["agent"]["vendor"].is_string()) {
        throw std::runtime_error("'agent:vendor' field is required.");
    }

    if (!config["agent"]["capabilities"].is_array()) {
        throw std::runtime_error("'agent:capabilities' field is required.");
    }

    if (!config["registration"].is_object()) {
        throw std::runtime_error("'registration' field is required.");
    }

    if (!config["registration"]["ipv4"].is_string()) {
        throw std::runtime_error("'registration:ipv4' field is required.");
    }

    if (!config["registration"]["port"].is_number()) {
        throw std::runtime_error("'registration:port' field is required.");
    }

    if (!config["registration"]["interval"].is_number()) {
        throw std::runtime_error("'registration:interval' field is required.");
    }

    if (!config["server"].is_object()) {
        throw std::runtime_error("'server' field is required.");
    }
    if (!config["server"]["port"].is_number()) {
        throw std::runtime_error("'server:port' field is required.");
    }

    if ((!config["database"].is_object()) || (!config["database"]["location"].is_string())) {
        throw std::runtime_error("Database location is not set.");
    }

    if (!config["certificate-files"].is_object()) {
        throw std::runtime_error("'certificate-files' field is required.");
    }
    if (!config["certificate-files"]["podm"].is_string()) {
        throw std::runtime_error("'certificate-files/podm' field is required.");
    }

    if (!config["managers"].is_array()) {
        throw std::runtime_error("'Managers' array is required.");
    }
    auto& managers_array = config["managers"].as_array();
    for (const auto& manager: managers_array) {
        if (!manager["device"].is_string()) {
            throw std::runtime_error("Each manager has to contain 'device' data.");
        }
        if (!manager["locationOffset"].is_uint()) {
            throw std::runtime_error("Each manager has to contain 'locationOffset' data.");
        }
    }
}

void configure_ipmi(const json::Value& config) {

    auto& managers_array = config["managers"].as_array();
    for (const auto& manager: managers_array) {
        IpmiConfig::get_instance().add_controller(uint8_t(manager["locationOffset"].as_uint()),
            std::make_shared<ipmi::manager::ipmitool::SerialController>(manager["device"].as_string()));
    }
}

void configure_certificates(agent::rmm::discovery::helpers::DiscoveryContext& dc, const json::Value& config) {
    dc.certificate_manager->set_cert_file_path(CertificateManager::CertificateType::PODM,
        config["certificate-files"]["podm"].as_string());
}

}

bool RmmLoader::load(const json::Value& config) {
    try {
        check_required_fields(config);
        configure_ipmi(config);
        configure_certificates(m_dc, config);
    }
    catch (const std::runtime_error& error) {
        log_error("rmm-agent", "Loading modules configuration failed: " << error.what());
        return false;
    }

    return true;
}
