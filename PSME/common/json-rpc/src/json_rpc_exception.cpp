/*!
 * @copyright Copyright (c) 2017-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file json_rpc_exception.cpp
 */

/*************************************************************************
 * libjson-rpc-cpp
 *************************************************************************
 * @file    exception.cpp
 * @date    31.12.2012
 * @author  Peter Spiess-Knafl <dev@spiessknafl.at>
 * @license See attached LICENSE.txt
 ************************************************************************/

#include "json-rpc/json_rpc_exception.hpp"

#include "json-rpc/json_rpc_error.hpp"

#include "json-rpc/common.hpp"

using namespace json_rpc;
using namespace json_rpc::common;

JsonRpcException::JsonRpcException(int code) :
        m_code(code), m_message(get_error_message(code)) {
    make_what();
}

JsonRpcException::JsonRpcException(const JsonRpcError& error) :
        m_code(error.get_code()),
        m_message(error.get_message()),
        m_data(error.get_data()) {
    make_what();
}

JsonRpcException::JsonRpcException(int code, const std::string& message) :
        m_code(code), m_message(message) {
    make_what();
}

JsonRpcException::JsonRpcException(int code, const std::string& message, const json::Json& data) :
        m_code(code), m_message(message), m_data(data) {
    make_what();
}

JsonRpcException::JsonRpcException(const std::string& message) :
        m_code(0), m_message(message) {
    make_what();
}

JsonRpcException::~JsonRpcException() {
}

void JsonRpcException::make_what() {
    if (m_code != 0) {
        std::stringstream ss{};
        ss << "Exception " << std::to_string(m_code) << " - " << get_error_message(m_code) << ": " << m_message;
        if (!m_data.empty()) {
            ss << ", data: " << m_data.dump();
        }
        m_what = ss.str();
    }
    else {
        m_what = m_message;
    }
}
