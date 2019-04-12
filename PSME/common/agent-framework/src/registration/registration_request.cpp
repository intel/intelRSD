/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 * */


#include "agent-framework/registration/registration_request.hpp"
#include "agent-framework/module/constants/psme.hpp"
#include "agent-framework/module/service_uuid.hpp"

#include "configuration/configuration.hpp"

using namespace agent_framework::generic;
using namespace agent_framework::model::literals;

RegistrationRequest::RegistrationRequest() {
    load_configuration();
}

json::Json RegistrationRequest::to_json() {
    return json::Json {
        {Attach::GAMI_ID, module::ServiceUuid::get_instance()->get_service_uuid()},
        {Attach::VERSION, m_version},
        {Attach::VENDOR, m_vendor},
        {Attach::IPV4_ADDRESS, m_ipv4address},
        {Attach::PORT, m_port},
        {Attach::CAPABILITIES, get_capabilities()}
    };
}

void RegistrationRequest::load_configuration() {
    const json::Json& configuration = configuration::Configuration::get_instance().to_json();

    auto ipv4_json = configuration.value("server", json::Json::object()).value("ipv4", json::Json::object());
    auto port_json = configuration.value("server", json::Json::object()).value("port", json::Json::object());
    auto type_json = configuration.value("agent", json::Json::object()).value("type", json::Json::object());
    auto vendor_json = configuration.value("agent", json::Json::object()).value("vendor", json::Json::object());
    auto capabilities = configuration.value("agent", json::Json::object()).value("capabilities", json::Json::object());

    if (ipv4_json.is_string()) {
        m_ipv4address = ipv4_json.get<std::string>();
    }

    if (port_json.is_number_unsigned()) {
        m_port = port_json.get<std::uint16_t>();
    }

    if (type_json.is_string()) {
        m_type = type_json.get<std::string>();
    }

    if (vendor_json.is_string()) {
        m_vendor = vendor_json.get<std::string>();
    }

    if (capabilities.is_array()) {
        for (const auto& capability : capabilities) {
            m_capabilities.emplace_back(Capability(capability.get<std::string>()));
        }
    }
}

json::Json RegistrationRequest::get_capabilities() {

    json::Json ret = json::Json::array();
    for (const auto& c : m_capabilities) {
        ret.emplace_back(c.get_name());
    }

    return ret;
}
