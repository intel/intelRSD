/*!
 * @copyright
 * Copyright (c) 2015-2018 Intel Corporation
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

#include "psme/rest/server/connector/connector.hpp"
#include "psme/rest/server/methods_handler.hpp"
#include "psme/rest/server/error/server_error.hpp"
#include "psme/rest/server/error/server_exception.hpp"
#include "psme/rest/server/error/error_factory.hpp"
#include "psme/core/agent/agent_unreachable.hpp"

#include <chrono>



using namespace psme::rest::server;
using namespace psme::rest::error;


Connector::Connector(const ConnectorOptions& options) :
    m_options(options),
    m_access_callback{[](const Request&, const Response&) { return true; }},
    m_callback{http_method_not_allowed} {}


Connector::~Connector() {}


void Connector::set_access_callback(const AccessCallback& access_callback) {
    m_access_callback = access_callback;
}


void Connector::set_callback(const Callback& callback) {
    m_callback = callback;
}


const ConnectorOptions& Connector::get_options() const {
    return m_options;
}


bool Connector::is_redirect_enabled() const {
    return (false == m_options.use_ssl() && m_options.get_redirect_port() > 0);
}


void Connector::handle(const Request& request, Response& response) noexcept {
    log_debug("rest", "\nRequest: "
        << "\n\tURL: " << request.get_url()
        << "\n\tMSG: " << request.get_body());

    auto started_at = std::chrono::high_resolution_clock::now();
    try {
        try_handle(request, response);
    }
    catch (const core::agent::AgentUnreachable& ex) {
        log_error("rest", "AgentUnreachable exception: " << ex.what());
        auto message = "Could not connect to underlying agent.";
        ServerError error = ErrorFactory::create_agent_unreachable_error(message);
        response.set_status(error.get_http_status_code());
        response.set_body(error.as_string());
    }
    catch (const agent_framework::exceptions::NotFound& ex) {
        log_error("rest", "Not found exception: " << ex.what());
        ServerError server_error = ErrorFactory::create_error_from_gami_exception(
            agent_framework::exceptions::NotFound(ex.get_message(), request.get_url())
        );
        response.set_status(server_error.get_http_status_code());
        response.set_body(server_error.as_string());
    }
    catch (const agent_framework::exceptions::GamiException& ex) {
        log_error("rest", "Agent framework exception: " << ex.what());
        ServerError server_error = ErrorFactory::create_error_from_gami_exception(ex);
        response.set_status(server_error.get_http_status_code());
        response.set_body(server_error.as_string());
    }
    catch (const ServerException& ex) {
        log_error("rest", "ServerException: " << ex.what());
        const auto& error = ex.get_error();
        response.set_status(error.get_http_status_code());
        response.set_body(error.as_string());
    }
    catch (const json::Value::Exception& ex) {
        log_error("rest", "JSON-cxx library exception on URL " << request.get_url() << ": " << ex.what());
        ServerError internal_server_error = ErrorFactory::create_internal_error();
        response.set_status(internal_server_error.get_http_status_code());
        response.set_body(internal_server_error.as_string());
    }
    catch (const std::exception& ex) {
        log_error("rest", "std::exception: " << ex.what());
        ServerError internal_server_error = ErrorFactory::create_internal_error();
        response.set_status(internal_server_error.get_http_status_code());
        response.set_body(internal_server_error.as_string());
    }
    catch (...) {
        log_error("rest", "Other exception.");
        ServerError internal_server_error = ErrorFactory::create_internal_error();
        response.set_status(internal_server_error.get_http_status_code());
        response.set_body(internal_server_error.as_string());
    }
    auto finished_at = std::chrono::high_resolution_clock::now();
    auto duration = finished_at - started_at;

    log_debug("rest", "\nResponse: "
        << "\n\tSTATUS: " << response.get_status()
        << "\n\tMSG: " << response.get_body()
        << "\n\tProcessing Time: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()
        << "ms");
}


void Connector::try_handle(const Request& request, Response& response) {
    if (m_access_callback(request, response)) {
        m_callback(request, response);
    }
    else {
        auto message = "Credentials do not have sufficient privileges for the requested operation.";
        throw ServerException(ErrorFactory::create_insufficient_privilege_error(message));
    }
}
