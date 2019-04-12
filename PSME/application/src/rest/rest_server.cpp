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
 * */

#include "psme/rest/rest_server.hpp"
#include "psme/rest/endpoints/endpoint_builder.hpp"
#include "psme/rest/server/multiplexer.hpp"
#include "configuration/configuration.hpp"
#include "logger/logger_factory.hpp"



using namespace psme::rest::server;


RestServer::RestServer() {
    const json::Json& config = configuration::Configuration::get_instance().to_json();
    auto connectors_options = load_connectors_options(config);

    endpoint::EndpointBuilder endpoint_builder;
    endpoint_builder.build_endpoints();

    ConnectorFactory connector_factory{};
    for (const auto& connector_options: connectors_options) {
        m_connectors.emplace_back(
            connector_factory.create_connector(connector_options,
                                               [](const Request& req, Response& res) {
                                                   Multiplexer::get_instance()->forward_to_handler(res,
                                                                                                   const_cast<Request&>(req));
                                               }));

        m_connectors.back()->set_unauthenticated_access_callback([](const std::string& http_method,
                                                                    const std::string& url,
                                                                    bool connection_secure) {
            return Multiplexer::get_instance()->check_public_access(http_method, url, connection_secure);
        });
    }
}


RestServer::~RestServer() {}


void RestServer::start() {
    log_info("rest", "Starting REST server ...");
    for (const auto& connector : m_connectors) {
        connector->start();
    }
    log_info("rest", "REST server started.");
}


void RestServer::stop() {
    log_info("rest", "Stopping REST server ...");
    for (const auto& connector : m_connectors) {
        connector->stop();
    }
    log_info("rest", "REST server stopped.");
}
