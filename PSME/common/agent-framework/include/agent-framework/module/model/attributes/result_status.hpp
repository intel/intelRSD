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
 *
 * @file result_status.hpp
 * @brief Declaration of setComponentAttributes command response item
 * */

#pragma once

#include "agent-framework/exceptions/error_codes.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace attribute {

/*! @brief setComponentAttributes response item */
class ResultStatus {
public:
    /*! Default constructor */
    ResultStatus() { }

    /*!
     * @brief Constructor
     * @param[in] attribute Name of the attribute
     * @param[in] code Error code
     * @param[in] message Result message
     * */
    ResultStatus(const std::string& attribute, exceptions::ErrorCode code, const std::string& message) :
        m_attribute(attribute), m_status_code(code), m_message(message) { }

    /*! Destructor */
    virtual ~ResultStatus();

    /*! Enable copy */
    ResultStatus(const ResultStatus&) = default;
    ResultStatus& operator=(const ResultStatus&) = default;
    ResultStatus(ResultStatus&&) = default;
    ResultStatus& operator=(ResultStatus&&) = default;

    /*!
     * @brief Set result status code
     * @param[in] code Error code
     * */
    void set_status_code(exceptions::ErrorCode code) {
        m_status_code = code;
    }

    /*!
     * @brief Get status code
     * @return Error code
     * */
    exceptions::ErrorCode get_status_code() const {
        return m_status_code;
    }

    /*!
     * @brief Set result message
     * @param[in] message Result message
     * */
    void set_message(const std::string& message) {
        m_message = message;
    }

    /*!
     * @brief Get result message
     * @return Result message
     * */
    const std::string& get_message() const {
        return m_message;
    }

    /*!
     * @brief Set attribute name
     * @param[in] attribute Attribute name
     * */
    void set_attribute(const std::string& attribute) {
        m_attribute = attribute;
    }

    /*!
     * @brief Get attribute name
     * @return Name of the attribute
     * */
    const std::string& get_attribute() const {
        return m_attribute;
    }

    /*!
     * @brief Transform the object to JSON
     * @return The object serialized to json::Json
     * */
    json::Json to_json() const;

    /*!
     * @brief Construct an object of class Result from JSON
     * @param json The json::Json to be deserialized
     * @return The newly constructed Result object
     * */
    static ResultStatus from_json(const json::Json& json);

private:
    std::string m_attribute{};
    exceptions::ErrorCode m_status_code{exceptions::ErrorCode::UNKNOWN_ERROR};
    std::string m_message{};
};

}
}
}
