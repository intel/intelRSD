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
 * @file json_rpc_error.hpp
 */

#pragma once

#include "json-rpc/common.hpp"

namespace json_rpc {

/*!
 * @brief Class represents a valid JsonRpcError. It is used for validation and (de)serialization purposes.
 */
class JsonRpcError final {
public:

    /*! Constructs a JsonRpcError based on the exception and id */
    JsonRpcError(const JsonRpcException& e, const json::Json& id):
        m_code(e.get_code()), m_message(e.get_message()), m_id(id), m_data(e.get_data()) {}

    /*! Constructs a full JsonRpcError: code, message, id, data fields */
    JsonRpcError(int code, const std::string& message, const json::Json& id, const json::Json& data):
        m_code(code), m_message(message), m_id(id), m_data(data) {}

    /*! Constructs a JsonRpcError without data field */
    JsonRpcError(int code, const std::string& message, const json::Json& id):
        m_code(code), m_message(message), m_id(id), m_data_present(false) {}

    /*! Enable copy */
    JsonRpcError(const JsonRpcError&) = default;
    JsonRpcError& operator=(const JsonRpcError&) = default;

    /*! Enable move */
    JsonRpcError(JsonRpcError&&) = default;
    JsonRpcError& operator=(JsonRpcError&&) = default;

    ~JsonRpcError() {}

    /*!
     * @brief JsonRpcError deserialization from Json, throws exceptions on errors
     * @param input Input json to be deserialized
     * @return Deserialized JsonRpcError
     */
    static JsonRpcError from_json(const json::Json& input);

    /*!
     * @brief JsonRpcError serialization to Json
     * @return Serialized JsonRpcError
     */
    json::Json to_json() const;

    /*!
     * @brief Gets the data field
     * @return JsonRpcError data field
     */
    const json::Json& get_data() const {
        return m_data;
    }

    /*!
     * @brief Gets the id field
     * @return JsonRpcError id field
     */
    const json::Json& get_id() const {
        return m_id;
    }

    /*!
     * @brief Gets the code field
     * @return JsonRpcError code field
     */
    int get_code() const {
        return m_code;
    }

    /*!
     * @brief Gets the message field
     * @return JsonRpcError message field
     */
    const std::string& get_message() const {
        return m_message;
    }

private:

    /* There are some additional fields in the class so it is not default-constructable */
    JsonRpcError() {}

    /* Throws if a json is not a valid JsonRpcError serialization json */
    static void throw_if_invalid_json(const json::Json& json);

    int m_code{};
    std::string m_message{};
    json::Json m_data{};
    json::Json m_id{};

    bool m_data_present{true};
};

}
