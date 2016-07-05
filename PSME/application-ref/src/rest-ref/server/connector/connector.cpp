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

#include "psme/rest-ref/server/connector/connector.hpp"
#include "psme/rest-ref/server/error/server_error.hpp"
#include "psme/rest-ref/server/error/server_exception.hpp"
#include "psme/rest-ref/server/error/error_factory.hpp"
#include "psme/core/agent/agent_unreachable.hpp"
#include "agent-framework/exceptions/not_found.hpp"
#include "logger/logger_factory.hpp"
#include <chrono>

using namespace psme::rest::server;
using psme::rest::error::ServerError;
using psme::rest::error::ServerException;
using psme::rest::error::ErrorFactory;

void psme::rest::server::default_callback(const Request&, Response& response) {
    auto error = ErrorFactory::create_method_not_allowed_error();
    response.set_status(error.get_http_status_code());
    response.set_body(error.as_string());
}

Connector::Connector(const ConnectorOptions& options)
    : m_options(options),
      m_access_callback{[](const Request&, const Response&) { return true;}},
      m_callback{default_callback} { }

Connector::~Connector() { }

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
    log_debug(GET_LOGGER("rest"), "\nRequest: " <<
                                  "\n\tURL: " << request.get_url() <<
                                  "\n\tMSG: " << request.get_body());

    auto started_at = std::chrono::high_resolution_clock::now();
    try {
        try_handle(request, response);
    }
    catch (const core::agent::AgentUnreachable& e) {
        log_error(GET_LOGGER("rest"),
                  std::string("AgentUnreachable exception: ") + e.what());
        ServerError error = ErrorFactory::create_agent_unreachable_error();
        response.set_status(error.get_http_status_code());
        response.set_body(error.as_string());
    }
    catch (const agent_framework::exceptions::NotFound& ex) {
        log_error(GET_LOGGER("rest"),
                  std::string("NotFound exception: ") + ex.what());
        ServerError not_found_error = ErrorFactory::create_not_found_error();
        not_found_error.add_extended_message(ex.get_message());
        response.set_status(not_found_error.get_http_status_code());
        response.set_body(not_found_error.as_string());
    }
    catch (const agent_framework::exceptions::InvalidParameters& ex) {
        log_error(GET_LOGGER("rest"),
                  std::string("Invalid parameters exception: ") + ex.what());
        ServerError invalid_payload_error =
                        ErrorFactory::create_invalid_payload_error();
        invalid_payload_error.add_extended_message(ex.get_message());
        response.set_status(invalid_payload_error.get_http_status_code());
        response.set_body(invalid_payload_error.as_string());
    }
    catch (const agent_framework::exceptions::UnsupportedValue& ex) {
        log_error(GET_LOGGER("rest"),
                  std::string("Unsupported value exception: ") + ex.what());
        ServerError invalid_payload_error =
                ErrorFactory::create_invalid_payload_error();
        invalid_payload_error.add_extended_message(ex.get_message());
        response.set_status(invalid_payload_error.get_http_status_code());
        response.set_body(invalid_payload_error.as_string());
    }
    catch (const agent_framework::exceptions::UnsupportedParameter& ex) {
        log_error(GET_LOGGER("rest"),
                  std::string("Unsupported parameter exception: ") + ex.what());
        ServerError invalid_payload_error =
                ErrorFactory::create_invalid_payload_error();
        invalid_payload_error.add_extended_message(ex.get_message());
        response.set_status(invalid_payload_error.get_http_status_code());
        response.set_body(invalid_payload_error.as_string());
    }
    catch (const agent_framework::exceptions::LvmError& ex) {
        log_error(GET_LOGGER("rest"),
                  std::string("Lvm exception: ") + ex.what());
        ServerError internal_server_error;
        internal_server_error.add_extended_message(ex.get_message());
        response.set_status(internal_server_error.get_http_status_code());
        response.set_body(internal_server_error.as_string());
    }
    catch (const agent_framework::exceptions::IscsiError& ex) {
        log_error(GET_LOGGER("rest"),
                  std::string("iSCSI exception: ") + ex.what());
        ServerError internal_server_error;
        internal_server_error.add_extended_message(ex.get_message());
        response.set_status(internal_server_error.get_http_status_code());
        response.set_body(internal_server_error.as_string());
    }
    catch (const agent_framework::exceptions::Fm10000Error& ex) {
        log_error(GET_LOGGER("rest"),
                  std::string("FM1000 exception: ") + ex.what());
        ServerError internal_server_error;
        internal_server_error.add_extended_message(ex.get_message());
        response.set_status(internal_server_error.get_http_status_code());
        response.set_body(internal_server_error.as_string());
    }
    catch (const agent_framework::exceptions::NotImplemented& ex) {
        log_error(GET_LOGGER("rest"),
                  std::string("Not implemented exception: ") + ex.what());
        ServerError not_implemented_error =
            ErrorFactory::create_agent_methond_not_implemented_error();
        not_implemented_error.add_extended_message(ex.get_message());
        response.set_status(not_implemented_error.get_http_status_code());
        response.set_body(not_implemented_error.as_string());
    }
    catch (const agent_framework::exceptions::InvalidUuid& ex) {
        log_error(GET_LOGGER("rest"),
                  std::string("Invalid uuid exception: ") + ex.what());
        ServerError internal_server_error;
        internal_server_error.add_extended_message(ex.get_message());
        response.set_status(internal_server_error.get_http_status_code());
        response.set_body(internal_server_error.as_string());
    }
    catch (const agent_framework::exceptions::CertificateError& ex) {
        log_error(GET_LOGGER("rest"),
                  std::string("Certificate error: ") + ex.what());
        ServerError internal_server_error;
        internal_server_error.add_extended_message(ex.get_message());
        response.set_status(internal_server_error.get_http_status_code());
        response.set_body(internal_server_error.as_string());
    }
    catch (const ServerException& e) {
        log_error(GET_LOGGER("rest"),
                  std::string("ServerException: ") + e.what());
        const ServerError &error = e.get_error();
        response.set_status(error.get_http_status_code());
        response.set_body(error.as_string());
    }
    catch (const std::exception& ex) {
        log_error(GET_LOGGER("rest"),
                  std::string("std::exception: ") + ex.what());
        ServerError internal_server_error;
        response.set_status(internal_server_error.get_http_status_code());
        response.set_body(internal_server_error.as_string());
    }
    catch (...) {
        log_error(GET_LOGGER("rest"), "Other exception.");
        ServerError internal_server_error;
        response.set_status(internal_server_error.get_http_status_code());
        response.set_body(internal_server_error.as_string());
    }
    auto finished_at = std::chrono::high_resolution_clock::now();
    auto duration = finished_at - started_at;

    log_debug(GET_LOGGER("rest"), "\nResponse: " <<
        "\n\tSTATUS: " << response.get_status() <<
        "\n\tMSG: " << response.to_buffer() <<
        "\n\tProcessing Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms");
}

void Connector::try_handle(const Request& request, Response& response) {
    if (m_access_callback(request, response)) {
        m_callback(request, response);
    }
    else {
/*      if (redirect_enabled()) {
            std::string location = build_location_header(
                                        request, true, request.get_url(),
                                        m_options.get_redirect_port());
            response.send_redirect(location);
        } else {
 */
            ServerError error{psme::rest::server::status_4XX::UNAUTHORIZED};
            response.set_status(error.get_http_status_code());
            response.set_body(error.as_string());
//      }
    }
}
