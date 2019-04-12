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
 * @file json_rpc_response.hpp
 */

#pragma once

#include "json-rpc/common.hpp"

namespace json_rpc {

/*!
 * @brief Class represents a valid JsonRpcResponse. It is used for validation and (de)serialization purposes.
 */
class JsonRpcResponse final {
public:

    /*! Constructs a full JsonRpcResponse with result and id fields */
    JsonRpcResponse(const json::Json& result, const json::Json& id):
        m_result(result), m_id(id) {}

    /*! Enable copy */
    JsonRpcResponse(const JsonRpcResponse&) = default;
    JsonRpcResponse& operator=(const JsonRpcResponse&) = default;

    /*! Enable move */
    JsonRpcResponse(JsonRpcResponse&&) = default;
    JsonRpcResponse& operator=(JsonRpcResponse&&) = default;

    ~JsonRpcResponse() {}

    /*!
     * @brief JsonRpcResponse deserialization from Json, throws exceptions on errors
     * @param input Input json to be deserialized
     * @return Deserialized JsonRpcResponse
     */
    static JsonRpcResponse from_json(const json::Json& input);

    /*!
     * @brief JsonRpcResponse serialization to Json
     * @return Serialized JsonRpcResponse
     */
    json::Json to_json() const;

    /*!
     * @brief Gets the result field
     * @return Result field
     */
    const json::Json& get_result() const {
        return m_result;
    }

    /*!
     * @brief Gets the id field
     * @return Id field
     */
    const json::Json& get_id() const {
        return m_id;
    }

private:

    /* There are some additional fields in the class so it is not default-constructible */
    JsonRpcResponse() {}

    /* Throws if a json is not a valid JsonRpcError serialization json */
    static void throw_if_invalid_json(const json::Json& json);

    json::Json m_id{};
    json::Json m_result{};
};

}
