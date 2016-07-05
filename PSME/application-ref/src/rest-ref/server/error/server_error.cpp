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
#include "psme/rest-ref/server/error/server_error.hpp"
#include "psme/rest-ref/server/status.hpp"

using namespace psme::rest::error;

namespace {
    constexpr char ERROR[] = "error";
    constexpr char CODE[] = "code";
    constexpr char MESSAGE[] = "message";
    constexpr char EXTENDED_INFO[] = "@Message.ExtendedInfo";
}

ServerError::~ServerError() { }

ServerError::ServerError()
    : m_http_status_code(psme::rest::server::status_5XX::INTERNAL_SERVER_ERROR),
      m_error(json::Value::Type::OBJECT) {
    set_code("Base.1.0.InternalError");
    set_message("The request failed due to an internal service error."
                " The service is still operational.");
    m_error[ERROR][EXTENDED_INFO] = json::Value::Type::ARRAY;
}

ServerError::ServerError(const std::uint32_t status_code)
    : m_http_status_code(status_code),
      m_error(json::Value::Type::OBJECT) {
    set_code(std::to_string(m_http_status_code));
    set_message(psme::rest::server::status::status_to_reason(m_http_status_code));
    m_error[ERROR][EXTENDED_INFO] = json::Value::Type::ARRAY;
}

void ServerError::set_http_status_code(const std::uint32_t status_code) {
    m_http_status_code = status_code;
}

void ServerError::set_code(const std::string& code) {
    m_error[ERROR][CODE] = code;
}

const std::string& ServerError::get_code() const {
    return m_error[ERROR][CODE].as_string();
}

void ServerError::set_message(const std::string& message) {
    m_error[ERROR][MESSAGE] = message;
}

const std::string& ServerError::get_message() const {
    return m_error[ERROR][MESSAGE].as_string();
}

void ServerError::add_extended_message(const std::string& info) {
    m_error[ERROR][EXTENDED_INFO].push_back(info);
}


std::string ServerError::as_string() const {
    return json::Serializer(m_error);
}
