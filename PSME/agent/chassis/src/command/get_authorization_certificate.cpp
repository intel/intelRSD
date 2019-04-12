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
 *
 * @file command/get_authorization_certificate.cpp
 *
 * @brief SDV chassis command get authorization certificate implementation
 * */

#include "agent-framework/module/chassis_components.hpp"
#include "agent-framework/command/registry.hpp"
#include "agent-framework/command/chassis_commands.hpp"

#include "ipmb/certificate_manager.hpp"
#include "base64/base64.hpp"

using namespace agent_framework::command;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent::chassis;

namespace {

    void get_authorization_certificate(
            const GetAuthorizationCertificate::Request& req,
            GetAuthorizationCertificate::Response& rsp) {

        AuthorizationCertificate certificate{};
        try {
            const auto& cert = ipmb::CertificateManager::get_instance()->
                get_certificate(ipmb::Certificate::from_string(req.get_certificate()));
            if (cert.get_data().empty() || !cert.is_complete()) {
                THROW(::agent_framework::exceptions::CertificateError, "agent",
                    req.get_certificate() + " certificate is not complete.");
            }
            certificate.set_certificate(base64::Base64::encodeUrl(cert.get_data()));
            certificate.set_certificate_hash(cert.get_hash());
            rsp = certificate;
            log_debug("agent", req.get_certificate()
                << " certificate was successfully read.");
        }
        catch (const std::runtime_error& re) {
            THROW(::agent_framework::exceptions::CertificateError, "agent", re.what());
        }
    }

}

REGISTER_COMMAND(GetAuthorizationCertificate, ::get_authorization_certificate);
