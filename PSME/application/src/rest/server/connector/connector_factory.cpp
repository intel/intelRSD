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
 **/

#include "psme/rest/server/connector/connector_factory.hpp"
#include "psme/rest/security/authentication/authentication_factory.hpp"
#include "psme/rest/server/connector/microhttpd/mhd_connector.hpp"



#ifdef ENABLE_HTTPS



#include "psme/rest/server/connector/microhttpd/mhd_ssl_connector.hpp"



#endif



#include "logger/logger_factory.hpp"



using namespace psme::rest::server;
using namespace psme::rest::security::authentication;

namespace {
ConnectorUPtr new_connector(const ConnectorOptions& connector_options) {
    ConnectorUPtr connector{};
    if (connector_options.use_ssl()) {
#ifdef ENABLE_HTTPS
        connector.reset(new MHDSSLConnector(connector_options));
#endif
    }
    else {
        connector.reset(new MHDConnector(connector_options));
    }
    if (!connector) {
        throw std::runtime_error("Failed to create connector."
                                 "Is HTTPS support compiled ?");
    }
    return connector;
}
}


ConnectorFactory::~ConnectorFactory() {}


ConnectorUPtr ConnectorFactory::create_connector(const ConnectorOptions& options,
                                                 const Connector::Callback& callback) const {
    auto connector = new_connector(options);
    connector->set_callback(callback);
    security::authentication::AuthenticationFactory authenticationFactory{};
    connector->set_authentication(authenticationFactory.create_authentication(options));
    return connector;
}
