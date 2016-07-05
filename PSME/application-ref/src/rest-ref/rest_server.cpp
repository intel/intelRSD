/*!
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
 * */

#include "psme/rest-ref/rest_server.hpp"
#include "psme/rest-ref/endpoints/endpoint_builder.hpp"
#include "configuration/configuration.hpp"
#include "logger/logger_factory.hpp"

using namespace psme::rest::server;

namespace {
    ConnectorOptionsVec load_connectors_options() {
        const json::Value& config =
                configuration::Configuration::get_instance().to_json();
        const auto& connectors_config = config["server"]["connectors"];
        ConnectorOptionsVec options{};
        for (const auto& connector_config : connectors_config) {
            options.emplace_back(ConnectorOptions{connector_config});
        }
        if (options.empty()) {
            throw std::runtime_error("No valid connector configuration found.");
        }
        return options;
    }
}

RestServer::RestServer() {
    auto options = load_connectors_options();

    endpoint::EndpointBuilder endpoint_builder;
    endpoint_builder.build_endpoints(m_mp);

    ConnectorFactory connector_factory{};
    for (const auto& connector_options: options) {
        m_connectors.emplace_back(
            connector_factory.create_connector(connector_options,
                [this](const Request& req, Response& res) {
                    m_mp.forward_to_handler(res, const_cast<Request&>(req));
                }));
    }
}

RestServer::~RestServer() { }

void RestServer::start() {
    log_info(GET_LOGGER("rest"), "Starting REST server ...");
    for (const auto& connector : m_connectors) {
        connector->start();
    }
    log_info(GET_LOGGER("rest"), "REST server started.");

    m_watcher.start();
    //m_polling.start();
}

void RestServer::stop() {
    log_info(GET_LOGGER("rest"), "Stopping REST server ...");
    for (const auto& connector : m_connectors) {
        connector->stop();
    }
    log_info(GET_LOGGER("rest"), "REST server stopped.");

    m_watcher.stop();
    //m_polling.stop();
}
