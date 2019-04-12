/*!
 * @copyright Copyright (c) 2016-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file validity_checker.hpp
 * */
#pragma once



#include "agent-framework/exceptions/exception.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include <memory>


namespace jsonrpc {

// Forward declaration
class ProcedureValidator;

/*!
 * @brief type mapping from validator to std types
 * @{
 */
using INT32 = std::int32_t;
using UINT32 = std::uint32_t;
using INT64 = std::int64_t;
using UINT64 = std::uint64_t;
using DOUBLE = double;
/*! @} */

/*!
 * @brief Type used to pass <procedure> to validate attribute
 */
using static_procedure_getter_t = const ProcedureValidator& (*)(void);

/*!
 * @brief Type used to pass value checker method to validate enum value
 */
using is_allowable_value_t = bool (*)(const std::string&);

/*!
 * @brief Type used to get all values defined in the enum
 */
using get_values_t = std::vector<std::string> (*)();


/*!
 * @brief interface to validate value from JSON document.
 *
 * All validity checkers inherit from this class.
 * */
class ValidityChecker {
public:

    /*! Internal exception to collect message and field name during validation */
    class ValidationException {
    public:

        /*!
         * @brief Constructor.
         * @param[in] code GAMI error code.
         * @param[in] message Error message.
         * @param[in] field_value Value of field.
         * @param[in] field Name of field.
         * */
        explicit ValidationException(agent_framework::exceptions::ErrorCode code, const std::string& message,
                                     const json::Json& field_value, const std::string& field = {});


        /*!
         * @brief Append subpath of property to path
         * @param[in] path Subpath of the property
         * */
        void append(const std::string& path);


        /*!
         * @brief Get exception message
         * @return Error message
         * */
        const std::string& get_message() const {
            return m_message;
        }


        /*!
         * @brief Get exception error code
         * @return Error Code
         * */
        agent_framework::exceptions::ErrorCode get_error_code() const {
            return m_code;
        }


        /*!
         * @brief Get invalid name of field
         * @return Field name
         * */
        const std::string& get_field() const {
            return m_field;
        }


        /*!
         * @brief Get invalid field value
         * @return Field value
         * */
        const json::Json& get_field_value() const {
            return m_field_value;
        }


    private:
        agent_framework::exceptions::ErrorCode m_code{};
        std::string m_field{};
        const json::Json m_field_value{};
        std::string m_message{};
    };


    /*! Destructor */
    virtual ~ValidityChecker();


    /*!
     * @brief validation method
     * @param value json value to be checked
     * @throws InvalidValue/InvalidField exception if not valid
     */
    virtual void validate(const json::Json& value) const noexcept(false);


protected:
    /*!
     * @brief Default constructor.
     *
     * Available only for subclasses, this is abstract class.
     */
    ValidityChecker() = default;


    /*!
     * @brief Procedure validator uses special <b>null</b> value to distinguish
     * from plain nulls.
     */
    friend class ProcedureValidator;

    /*!
     * @brief special value for fields not in the object, to distinguish from
     * plain nulls
     * */
    static const json::Json NON_EXISTING_VALUE;

    /*! @brief Type alias for custom validity checkers */
    using Ptr = std::unique_ptr<ValidityChecker>;

private:

    /*! Copy constructor. Not allowed */
    ValidityChecker(const ValidityChecker&) = delete;


    /*! Assign operator. Not allowed */
    ValidityChecker& operator=(const ValidityChecker&) = delete;
};

}
