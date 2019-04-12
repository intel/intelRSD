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
#include "psme/rest/server/error/message_object.hpp"
#include "psme/rest/server/error/server_error.hpp"
#include "psme/rest/server/status.hpp"
#include "psme/rest/constants/constants.hpp"



using namespace psme::rest::error;
using namespace psme::rest::constants;

const constexpr char ServerError::INTERNAL_ERROR[];
const constexpr char ServerError::RESOURCE_IN_USE[];
const constexpr char ServerError::GENERAL_ERROR[];
const constexpr char ServerError::MALFORMED_JSON[];
const constexpr char ServerError::RESOURCE_ALREADY_EXISTS[];
const constexpr char ServerError::RESOURCE_MISSING_AT_URI[];
const constexpr char ServerError::RESOURCE_AT_URI_UNAUTHORIZED[];
const constexpr char ServerError::INSUFFICIENT_PRIVILEGE[];
const constexpr char ServerError::SERVICE_TEMPORARILY_UNAVAILABLE[];
const constexpr char ServerError::PROPERTY_UNKNOWN[];
const constexpr char ServerError::PROPERTY_MISSING[];
const constexpr char ServerError::PROPERTY_VALUE_TYPE_ERROR[];
const constexpr char ServerError::PROPERTY_VALUE_NOT_IN_LIST[];
const constexpr char ServerError::PROPERTY_VALUE_FORMAT_ERROR[];
const constexpr char ServerError::PROPERTY_DUPLICATE[];
const constexpr char ServerError::PROPERTY_NOT_MODIFIABLE[];
const constexpr char ServerError::PROPERTY_VALUE_RESTRICTED[];


ServerError::~ServerError() {}


ServerError::ServerError(const std::uint32_t status_code) :
    m_http_status_code(status_code),
    m_error(json::Json::value_t::object) {

    m_error[ErrorMessage::ERROR][ErrorMessage::CODE] = json::Json();
    m_error[ErrorMessage::ERROR][ErrorMessage::MESSAGE] = json::Json();
    m_error[ErrorMessage::ERROR][ErrorMessage::EXTENDED_INFO] = json::Json::array();
}


void ServerError::set_http_status_code(const std::uint32_t status_code) {
    m_http_status_code = status_code;
}


void ServerError::set_code(const std::string& code) {
    m_error[ErrorMessage::ERROR][ErrorMessage::CODE] = code;
}


std::string ServerError::get_code() const {
    return m_error.value(ErrorMessage::ERROR, json::Json::object()).value(ErrorMessage::CODE, std::string{});
}


void ServerError::set_message(const std::string& message) {
    m_error[ErrorMessage::ERROR][ErrorMessage::MESSAGE] = message;
}


std::string ServerError::get_message() const {
    return m_error.value(ErrorMessage::ERROR, json::Json::object()).value(ErrorMessage::MESSAGE, std::string{});
}


void ServerError::add_extended_message(const std::string& info) {
    add_extended_message(get_code(), info);
}


void ServerError::add_extended_message(const std::string& message_id, const std::string& info) {
    MessageObject message{message_id, info};
    add_extended_message(message);
}


void ServerError::add_extended_message(const MessageObject& object) {
    m_error[ErrorMessage::ERROR][ErrorMessage::EXTENDED_INFO].push_back(object);
}


bool ServerError::has_extended_messages() const {
    return !m_error[ErrorMessage::ERROR][ErrorMessage::EXTENDED_INFO].empty();
}


std::string ServerError::as_string() const {
    return m_error.dump();
}


json::Json ServerError::get_extended_messages() const {
    return m_error[ErrorMessage::ERROR][ErrorMessage::EXTENDED_INFO];
}
