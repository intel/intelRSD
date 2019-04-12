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
 * @file gami_exception.hpp
 *
 * @brief Declaration of GAMI Exception base class interface
 * */

#pragma once



#include "agent-framework/exceptions/error_codes.hpp"
#include "json-rpc/json_rpc_exception.hpp"


namespace agent_framework {
namespace exceptions {

/*! @brief Base exception implementation */
class GamiException : public json_rpc::JsonRpcException {
public:

    /*!
     * @brief Create exception object
     * @param[in] code error code
     * @param[in] message error message
     * */
    explicit GamiException(const ErrorCode code, const std::string& message);


    /*!
     * @brief Create exception object
     * @param[in] code error code
     * @param[in] message error message
     * @param[in] data JSON object containing additional data
     * */
    explicit GamiException(const ErrorCode code, const std::string& message, const json::Json& data);


    GamiException(const GamiException&) = default;


    GamiException& operator=(const GamiException&) = default;


    GamiException(GamiException&&) = default;


    GamiException& operator=(GamiException&&) = default;


    /*! @brief Destructor */
    virtual ~GamiException();


    /*!
     * @brief Get error code
     * @return Error code
     */
    virtual ErrorCode get_error_code() const;


    /*!
     * @brief Get exception message
     * @return ExceptionBase message
     */
    virtual const std::string& get_message() const;


    /*!
     * @brief Get exception JSON data
     * @return ExceptionBase JSON data
     */
    virtual const json::Json& get_data() const;


protected:

    /*! Default message for not specified field in JSON data of exception */
    static const constexpr char NOT_SPECIFIED[] = "<not specified>";

    /*!
     * @brief Get JSON object from JSON data.
     * @param data Exception's JSON data.
     * @param field_key JSON key.
     * @return String from JSON data.
     * */
    static json::Json get_json_from_data(const json::Json& data, const std::string& field_key);

    /*!
     * @brief Get styled string from JSON data.
     * @param data Exception's JSON data.
     * @param field_key JSON key.
     * @return String from JSON data.
     * */
    static std::string get_styled_string_from_data(const json::Json& data, const std::string& field_key);

    /*!
     * @brief Get string from JSON data.
     * @param data Exception's JSON data.
     * @param field_key JSON key.
     * @return String from JSON data.
     * */
    static std::string get_string_from_data(const json::Json& data, const std::string& field_key);

private:
    const ErrorCode m_error_code;
    const std::string m_message;
};

} // namespace exceptions
} // namespace agent_framework
