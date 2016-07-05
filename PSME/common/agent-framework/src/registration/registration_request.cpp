/*!
 * @section LICENSE
 *
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
 *
 * @section DESCRIPTION
 * */


#include "agent-framework/registration/registration_request.hpp"
#include "agent-framework/registration/service_uuid.hpp"
#include "agent-framework/logger_ext.hpp"

#include "configuration/configuration.hpp"

using configuration::Configuration;

using namespace agent_framework::generic;

RegistrationRequest::RegistrationRequest() {
    load_configuration();
}

Json::Value RegistrationRequest::to_json() {
    Json::Value request;
    request["gamiId"] = ServiceUuid::get_instance()->get_service_uuid();
    request["version"] = m_version;
    request["vendor"] = m_vendor;
    request["ipv4address"] = m_ipv4address;
    request["port"] = m_port;
    request["capabilities"] = get_capabilities();
    return request;
}

void RegistrationRequest::load_configuration() {
    const json::Value& configuration = Configuration::get_instance().to_json();

    auto ipv4_json = configuration["server"]["ipv4"];
    auto port_json = configuration["server"]["port"];
    auto type_json = configuration["agent"]["type"];
    auto vendor_json = configuration["agent"]["vendor"];
    auto capabilities = configuration["agent"]["capabilities"];

    if (ipv4_json.is_string()) {
        m_ipv4address = ipv4_json.as_string();
    }

    if (port_json.is_uint()) {
        m_port = static_cast<std::uint16_t>(port_json.as_uint());
    }

    if (type_json.is_string()) {
        m_type = type_json.as_string();
    }

    if (vendor_json.is_string()) {
        m_vendor = vendor_json.as_string();
    }

    if (capabilities.is_array()) {
        for(auto capability : capabilities) {
            m_capabilities.emplace_back(
                    Capability(capability["name"].as_string()));
        }
    }
}

Json::Value RegistrationRequest::get_capabilities() {

    Json::Value ret{Json::arrayValue};
    for (const auto& c : m_capabilities) {
        Json::Value capability;
        capability = c.get_name();
        ret.append(std::move(capability));
    }

    return ret;
}

