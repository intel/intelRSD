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
 * @file handlers/json_rpc_request_handler.hpp
 */

#pragma once

#include "json-rpc/handlers/abstract_request_handler.hpp"
#include "json-rpc/common.hpp"

#include <map>

namespace json_rpc {

/*!
 * @brief Implementation of AbstractRequestHandler interface for Json RPC servers
 */
class JsonRpcRequestHandler : public AbstractRequestHandler {
public:

    /*! Method handler signature */
    using MethodHandler = std::function<json::Json(const json::Json&)>;

    /*! Notification handler signature */
    using NotificationHandler = std::function<void(const json::Json&)>;

    /*! Logger function signature*/
    using LoggerFunction = std::function<void(const std::string&)>;

    JsonRpcRequestHandler();

    virtual ~JsonRpcRequestHandler();

    /*!
     * @brief Sets handler for a specific method call
     * @param method Name of the method
     * @param handler Handler to be used
     */
    void set_method_handler(const std::string& method, const MethodHandler& handler) {
        m_method_handlers[method] = handler;
    }

    /*!
     * @brief Sets handler for a specific notification call
     * @param method Name of the notification
     * @param handler Handler to be used
     */
    void set_notification_handler(const std::string& method, const NotificationHandler& handler) {
        m_notification_handlers[method] = handler;
    }

    /*!
     * @brief Return currently used handler for a specific method
     * @param method Method name
     * @return Method handler
     */
    MethodHandler get_method_handler(const std::string& method) const {
        if (m_method_handlers.find(method) != m_method_handlers.end()) {
            return m_method_handlers.at(method);
        }
        throw JsonRpcException(common::ERROR_RPC_METHOD_NOT_FOUND, "Unknown method: " + method);
    }

    /*!
     * @brief Return currently used handler for a specific notification
     * @param method Notification name
     * @return Notification handler
     */
    NotificationHandler get_notification_handler(const std::string& method) const {
        if (m_notification_handlers.find(method) != m_notification_handlers.end()) {
            return m_notification_handlers.at(method);
        }
        throw JsonRpcException(common::ERROR_RPC_METHOD_NOT_FOUND, "Unknown notification: " + method);
    }

    /*!
     * @brief Handle request
     * @param input Request message
     * @return Request response
     */
    virtual std::string handle(const std::string& input) override;

    /*!
     * @brief Sets logger used for info messages
     * @param logger New logger to be used
     */
    void set_info_logger(const LoggerFunction& logger) {
        m_info_logger = logger;
    }

    /*!
     * @brief Sets logger used for error messages
     * @param logger New logger to be used
     */
    void set_error_logger(const LoggerFunction& logger) {
        m_error_logger = logger;
    }

protected:

    /* Uses info logger to print message */
    void show_info(const std::string& msg) const {
        if (m_info_logger) {
            m_info_logger(msg);
        }
    }

    /* Uses error logger to print message */
    void show_error(const std::string& msg) const {
        if (m_error_logger) {
            m_error_logger(msg);
        }
    }

private:

    /* Handle signle Json RPC request (Json RPC call may be a batch request consisting of several requests) */
    bool handle_single_request(const json::Json&, json::Json&);

    std::map<std::string, MethodHandler> m_method_handlers{};
    std::map<std::string, NotificationHandler> m_notification_handlers{};

    LoggerFunction m_info_logger{};
    LoggerFunction m_error_logger{};

};

}
