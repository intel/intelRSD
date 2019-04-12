/*!
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
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
 **/

#include "psme/rest/security/authentication/authentication_factory.hpp"
#include "psme/rest/security/authentication/basic_authentication.hpp"
#include "psme/rest/security/authentication/session_authentication.hpp"
#include "psme/rest/security/authentication/client_cert_authentication.hpp"



using namespace psme::rest::security::authentication;
using namespace psme::rest::server;

namespace {
std::vector<AuthenticationUPtr> new_authentication(const ConnectorOptions& connector_options) {
    std::vector<AuthenticationUPtr> authentications;
    if (connector_options.get_authentication_type() ==
        ConnectorOptions::AuthenticationType::REDFISH_SESSION_AUTH) {
        authentications.emplace_back(new SessionAuthentication());
    }
    else if (connector_options.get_authentication_type() ==
             ConnectorOptions::AuthenticationType::BASIC_AUTH) {
        authentications.emplace_back(new BasicAuthentication());
    }
    else if (connector_options.get_authentication_type() ==
             ConnectorOptions::AuthenticationType::BASIC_AUTH_OR_REDFISH_SESSION_AUTH) {
        authentications.emplace_back(new BasicAuthentication());
        authentications.emplace_back(new SessionAuthentication());
    }
    if (authentications.empty()) {
        log_warning("rest", "Authentication for " << "HTTP" << (connector_options.use_ssl() ? "S" : "")
                                                  << " connector disabled\n");
    }
    return authentications;
}
}


AuthenticationFactory::~AuthenticationFactory() {}


std::vector<AuthenticationUPtr> AuthenticationFactory::create_authentication(const ConnectorOptions& options) const {
    return new_authentication(options);
}
