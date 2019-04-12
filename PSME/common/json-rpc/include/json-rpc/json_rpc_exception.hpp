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
 * @file json_rpc_exception.hpp
 */

/*************************************************************************
 * libjson-rpc-cpp
 *************************************************************************
 * @file    exception.h
 * @date    31.12.2012
 * @author  Peter Spiess-Knafl <dev@spiessknafl.at>
 * @license See attached LICENSE.txt
 ************************************************************************/

#pragma once

#include "json-wrapper/json-wrapper.hpp"

namespace json_rpc {

class JsonRpcError;

/*! Json Rpc Exception class - represents error during Json Rpc operation, convertable to JsonRpcError */
class JsonRpcException : public std::exception {
public:

    /*! Constructs JsonRpcException based on the error code */
    explicit JsonRpcException(int code);

    /*! Constructs JsonRpcException based on the JsonRpcError response */
    explicit JsonRpcException(const JsonRpcError& error);

    /*! Constructs JsonRpcException based on the message, using a default error code */
    explicit JsonRpcException(const std::string& message);

    /*! Constructs JsonRpcException based on the code and message */
    JsonRpcException(int code, const std::string& message);

    /*! Constructs JsonRpcException based on the code, message and data fields */
    JsonRpcException(int code, const std::string& message, const json::Json& data);

    /*! Enable copy */
    JsonRpcException(const JsonRpcException&) = default;
    JsonRpcException& operator=(const JsonRpcException&) = default;

    /*! Enable move */
    JsonRpcException(JsonRpcException&&) = default;
    JsonRpcException& operator=(JsonRpcException&&) = default;

    virtual ~JsonRpcException();

    /*!
     * @brief Gets the code field
     * @return Code field
     */
    int get_code() const {
        return m_code;
    }

    /*!
     * @brief Sets the code field
     * @param code New value
     */
    void set_code(int code) {
        m_code = code;
    }

    /*!
     * @brief Gets the message field
     * @return Message field
     */
    const std::string& get_message() const {
        return m_message;
    }

    /*!
     * @brief Sets the message field
     * @param message New value
     */
    void set_message(const std::string& message) {
        m_message = message;
    }

    /*!
     * @brief Gets the data field
     * @return Data field
     */
    const json::Json& get_data() const {
        return m_data;
    }

    /*!
     * @brief Sets the data field
     * @param data New value
     */
    void set_data(const json::Json& data) {
        m_data = data;
    }

    /*!
     * @brief Gets the exception 'what' message
     * @return What message
     */
    virtual const char* what() const noexcept override {
        return m_what.c_str();
    }

private:

    /* Prepares m_what field returned on what() method call */
    void make_what();

    int m_code{};
    std::string m_message{};
    std::string m_what{};
    json::Json m_data{};
};

}
