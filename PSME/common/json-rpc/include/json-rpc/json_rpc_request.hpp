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
 * @file json_rpc_request.hpp
 */

#pragma once

#include "json-rpc/common.hpp"

namespace json_rpc {

/*!
 * @brief Class represents a valid JsonRpcRequest. It is used for validation and (de)serialization purposes.
 */
class JsonRpcRequest final {
public:

    /*! Enable copy */
    JsonRpcRequest(const JsonRpcRequest&) = default;
    JsonRpcRequest& operator=(const JsonRpcRequest&) = default;

    /*! Enable move */
    JsonRpcRequest(JsonRpcRequest&&) = default;
    JsonRpcRequest& operator=(JsonRpcRequest&&) = default;

    ~JsonRpcRequest() {}

    /*!
     * @brief Constructs a valid method request with method/id/params fields
     * @param method Name of the method
     * @param id Id of the request
     * @param params Parameters of the request
     * @return Constructed JsonRpcRequest
     */
    static JsonRpcRequest make_method_request(const std::string& method, const json::Json& id, const json::Json& params) {
        JsonRpcRequest req{};
        req.m_method = method;
        req.m_params = params;
        req.m_id = id;
        return req;
    }

    /*!
     * @brief Constructs a valid method request with method/id fields
     * @param method Name of the method
     * @param id Id of the request
     * @return Constructed JsonRpcRequest
     */
    static JsonRpcRequest make_method_request(const std::string& method, const json::Json& id) {
        JsonRpcRequest req{};
        req.m_method = method;
        req.m_id = id;
        req.m_params_present = false;
        return req;
    }

    /*!
     * @brief Constructs a valid notification request with method/params fields
     * @param method Name of the method
     * @param params Parameters of the request
     * @return Constructed JsonRpcRequest
     */
    static JsonRpcRequest make_notification_request(const std::string& method, const json::Json& params) {
        JsonRpcRequest req{};
        req.m_method = method;
        req.m_params = params;
        req.m_id_present = false;
        return req;
    }

    /*!
     * @brief Constructs a valid notification request with the method field
     * @param method Name of the method
     * @return Constructed JsonRpcRequest
     */
    static JsonRpcRequest make_notification_request(const std::string& method) {
        JsonRpcRequest req{};
        req.m_method = method;
        req.m_id_present = false;
        req.m_params_present = false;
        return req;
    }

    /*!
     * @brief JsonRpcRequest deserialization from Json, throws exceptions on errors
     * @param input Input json to be deserialized
     * @return Deserialized JsonRpcRequest
     */
    static JsonRpcRequest from_json(const json::Json& input);

    /*!
     * @brief JsonRpcRequest serialization to Json
     * @return Serialized JsonRpcRequest
     */
    json::Json to_json() const;

    /*!
     * @brief Gets the method field
     * @return Method field
     */
    const std::string& get_method() const {
        return m_method;
    }


    /*!
     * @brief Gets the id field
     * @return id field
     */
    const json::Json& get_id() const {
        return m_id;
    }

    /*!
     * @brief Gets the params
     * @return Params field
     */
    const json::Json& get_params() const {
        return m_params;
    }

    /*!
     * @brief Checks if request has filled params field
     * @return True if params were set
     */
    bool has_params() const {
        return m_params_present;
    }

    /*!
     * @brief Checks if this request is a notification
     * @return True for JsonRpcRequest representing notifications
     */
    bool is_notification() const {
        return !m_id_present;
    }

private:

    /* There are some additional fields in the class so it is not default-constructible */
    JsonRpcRequest() {}

    /* Throws if a json is not a valid JsonRpcError serialization json */
    static void throw_if_invalid_json(const json::Json& json);

    std::string m_method{};
    json::Json m_id{};
    json::Json m_params{};

    bool m_params_present{true};
    bool m_id_present{true};
};

}
