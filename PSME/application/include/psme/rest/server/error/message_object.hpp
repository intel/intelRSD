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
 * @file message_object.hpp
 * @brief Declaration of REST API Error Message Object class
 * */

#pragma once



#include <string>
#include <vector>

#include "agent-framework/module/enum/enum_builder.hpp"
#include "json-wrapper/json-wrapper.hpp"

namespace psme {
namespace rest {
namespace error {

/*! @brief Redfish Severity enum definition */
ENUM(Severity, std::uint32_t, Critical, Warning, OK);

class MessageObject {
public:

    /*!
     * @brief Constructor
     * @param[in] message_id String indicating a specific error or message.
     * @param[in] message A human readable error message indicating the semantics associated with the error.
     * */
    MessageObject(const std::string& message_id, const std::string& message) :
        m_message_id(message_id), m_message(message) {}


    /*!
     * @brief Gets a string indicating a specific error or message (not to be confused with the HTTP status code).
     * This code can be used to access a detailed message from a message registry.
     *
     * @return Error message identifier.
     * */
    const std::string& get_message_id() const {
        return m_message_id;
    }


    /*!
     * @brief Gets a human readable error message indicating the semantics associated with the error.
     * This shall be the complete message, and not rely on substitution variables.
     *
     * @return Error message as std::string.
     * */
    const std::string& get_message() const {
        return m_message;
    }


    /*!
     * @brief Add message argument to REST API MessageObject.
     * @param[in] argument Message argument representing parameter values for the message.
     * */
    void add_message_argument(const std::string& argument) {
        m_message_args.push_back(argument);
    }


    /*!
     * @brief Gets an optional array of strings representing the substitution parameter values for the message.
     * This shall be included in the response if a MessageId is specified for a parameterized message.
     *
     * @return Array of strings representing the substitution parameter values for the message.
     * */
    const std::vector<std::string>& get_message_arguments() const {
        return m_message_args;
    }


    /*!
     * @brief Sets severity of the error.
     * @param[in] severity Severity of the error.
     * */
    void set_severity(Severity severity) {
        m_severity = severity;
    }


    /*!
     * @brief Gets an optional string representing the severity of the error.
     * @return Severity of the error.
     * */
    Severity get_severity() const {
        return m_severity;
    }


    /*!
     * @brief Sets recommended action(s) to take to resolve the error.
     * @param[in] resolution The resolution of the error.
     * */
    void set_resolution(const std::string& resolution) {
        m_resolution = resolution;
    }


    /*!
     * @brief Gets an optional string describing recommended action(s) to take to resolve the error.
     * @return The resolution of the error.
     * */
    const std::string& get_resolution() const {
        return m_resolution;
    }


    /*!
     * @brief Sets an array of JSON Pointer to error-related properties.
     * @param[in] related_properties Vector of JSON Pointer to properties related with the error.
     * */
    void set_related_properties(const std::vector<std::string>& related_properties);

    /*!
     * @brief Add a JSON Pointer to related properties array.
     * @param[in] related_property JSON Pointer to property related with the error.
     * */
    void add_related_property(const std::string& related_property);


    /*!
     * @brief Gets an optional array of JSON Pointers defining the specific properties
     * within a JSON payload describing by the message.
     *
     * @return Array of related properties.
     * */
    const std::vector<std::string>& get_related_properties() const {
        return m_related_properties;
    }


    /*! @brief Conversion operator to json::Json */
    operator json::Json() const;


private:
    std::string m_message_id{};
    std::string m_message{};
    std::vector<std::string> m_message_args{};
    Severity m_severity{Severity::Warning};
    std::string m_resolution{};
    std::vector<std::string> m_related_properties{};

};

}
}
}
