/*!
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
 * @file rmm/loader/rmm_loader.cpp
 */

#include "loader/rmm_loader.hpp"
#include "loader/ipmi_config.hpp"

#include "agent-framework/version.hpp"
#include "agent-framework/module/service_uuid.hpp"

#include "certificate_management/certificate_manager.hpp"
#include "ipmi/manager/ipmitool/serial_controller.hpp"
#include "configuration/configuration.hpp"
#include "logger/logger.hpp"


using namespace agent::rmm::loader;
using namespace agent::rmm;

namespace {

void check_required_fields(const json::Json& config) {
    if (!config.value("service", json::Json()).is_string()) {
        throw std::runtime_error("'service' field is required.");
    }

    if (!config.value("agent", json::Json()).is_object()) {
        throw std::runtime_error("'agent' field is required.");
    }

    if (!config["agent"].value("vendor", json::Json()).is_string()) {
        throw std::runtime_error("'agent:vendor' field is required.");
    }

    if (!config["agent"].value("capabilities", json::Json()).is_array()) {
        throw std::runtime_error("'agent:capabilities' field is required.");
    }

    if (!config.value("registration", json::Json()).is_object()) {
        throw std::runtime_error("'registration' field is required.");
    }

    if (!config["registration"].value("ipv4", json::Json()).is_string()) {
        throw std::runtime_error("'registration:ipv4' field is required.");
    }

    if (!config["registration"].value("port", json::Json()).is_number()) {
        throw std::runtime_error("'registration:port' field is required.");
    }

    if (!config["registration"].value("interval", json::Json()).is_number()) {
        throw std::runtime_error("'registration:interval' field is required.");
    }

    if (!config.value("server", json::Json()).is_object()) {
        throw std::runtime_error("'server' field is required.");
    }
    if (!config["server"].value("port", json::Json()).is_number()) {
        throw std::runtime_error("'server:port' field is required.");
    }

    if ((!config.value("database", json::Json::object()).value("location", json::Json()).is_string())) {
        throw std::runtime_error("Database location is not set.");
    }

    if (!config.value("certificate-files", json::Json()).is_object()) {
        throw std::runtime_error("'certificate-files' field is required.");
    }
    if (!config["certificate-files"].value("podm", json::Json()).is_string()) {
        throw std::runtime_error("'certificate-files/podm' field is required.");
    }

    if (!config.value("managers", json::Json()).is_array()) {
        throw std::runtime_error("'Managers' array is required.");
    }

    for (const auto& manager: config["managers"]) {
        if (!manager.value("device", json::Json()).is_string()) {
            throw std::runtime_error("Each manager has to contain 'device' data.");
        }
        if (!manager.value("locationOffset", json::Json()).is_number_unsigned()) {
            throw std::runtime_error("Each manager has to contain 'locationOffset' data.");
        }
    }

}

void configure_ipmi(const json::Json& config) {
    if (config.count("managers")) {
        const auto& managers_array = config["managers"];
        for (const auto& manager: managers_array) {
            IpmiConfig::get_instance().add_controller(manager["locationOffset"].get<std::uint8_t>(),
                                                      std::make_shared<ipmi::manager::ipmitool::SerialController>(
                                                          manager["device"].get<std::string>()));
        }
    }
}

void configure_certificates(agent::rmm::discovery::helpers::DiscoveryContext& dc, const json::Json& config) {
    dc.certificate_manager->set_cert_file_path(CertificateManager::CertificateType::PODM,
                                               config["certificate-files"]["podm"].get<std::string>());
}

}

bool RmmLoader::load(const json::Json& config) {
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
