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

#include "psme/rest-ref/server/connector/connector_options.hpp"

using namespace psme::rest::server;

constexpr const char ConnectorOptions::PORT[];
constexpr const char ConnectorOptions::REDIRECT_PORT[];
constexpr const char ConnectorOptions::USE_SSL[];
constexpr const char ConnectorOptions::CERTS_DIR[];
constexpr const char ConnectorOptions::CLIENT_CERT_REQUIRED[];
constexpr const char ConnectorOptions::THREAD_MODE[];
constexpr const char ConnectorOptions::THREAD_MODE_SELECT[];
constexpr const char ConnectorOptions::THREAD_MODE_THREAD_PER_CONNECTION[];
constexpr const char ConnectorOptions::THREAD_POOL_SIZE[];
constexpr const char ConnectorOptions::DEBUG_MODE[];

ConnectorOptions::ConnectorOptions(const json::Value& config) {
    m_port = static_cast<uint16_t> (config[PORT].as_uint());
    if (config.is_member(REDIRECT_PORT)) {
        m_redirect_port = static_cast<uint16_t> (config[REDIRECT_PORT].as_uint());
    }
    const auto& thread_mode = config[THREAD_MODE].as_string();
    if (thread_mode == THREAD_MODE_SELECT) {
        m_thread_mode = ThreadMode::SELECT;
    } else if (thread_mode == THREAD_MODE_THREAD_PER_CONNECTION) {
        m_thread_mode = ThreadMode::THREAD_PER_CONNECTION;
    }
    if (config.is_member(DEBUG_MODE)) {
        m_use_debug = config[DEBUG_MODE].as_bool();
    }
    if (config.is_member(THREAD_POOL_SIZE)) {
        m_thread_pool_size = config[THREAD_POOL_SIZE].as_uint();
    }
    m_use_ssl = config[USE_SSL].as_bool();
    if (m_use_ssl) {
        m_certs_dir = config[CERTS_DIR].as_string();

        if (config.is_member(CLIENT_CERT_REQUIRED)) {
            m_is_client_cert_required = config[CLIENT_CERT_REQUIRED].as_bool();
        }
    }
}

ConnectorOptions::~ConnectorOptions() { }

bool ConnectorOptions::use_ssl() const {
    return m_use_ssl;
}

const std::string& ConnectorOptions::get_certs_dir() const {
    return m_certs_dir;
}

uint16_t ConnectorOptions::get_port() const {
    return m_port;
}

uint16_t ConnectorOptions::get_redirect_port() const {
    return m_redirect_port;
}

bool ConnectorOptions::is_client_cert_required() const {
    return m_is_client_cert_required;
}

ConnectorOptions::ThreadMode ConnectorOptions::get_thread_mode() const {
    return m_thread_mode;
}

unsigned int ConnectorOptions::get_thread_pool_size() const {
    return m_thread_pool_size;
}

bool ConnectorOptions::use_debug() const {
    return m_use_debug;
}
