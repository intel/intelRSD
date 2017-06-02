/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * */

#pragma once



#include "psme/rest/server/request.hpp"
#include "agent-framework/validators/procedure_validator.hpp"

#include <algorithm>

namespace json {
/* Forward declaration */
class Value;
}

namespace psme {
namespace rest {
namespace validators {

class JsonValidator {
public:
    static constexpr const char STRING_TYPE[] __attribute__ ((__deprecated__)) = "string";
    static constexpr const char ARRAY_TYPE[] __attribute__ ((__deprecated__)) = "array";
    static constexpr const char DOUBLE_TYPE[] __attribute__ ((__deprecated__)) = "double";
    static constexpr const char BOOL_TYPE[] __attribute__ ((__deprecated__)) = "bool";
    static constexpr const char UINT_TYPE[] __attribute__ ((__deprecated__)) = "uint";
    static constexpr const char OBJECT_TYPE[] __attribute__ ((__deprecated__)) = "object";
    static constexpr const char NUMBER_TYPE[] __attribute__ ((__deprecated__)) = "number";


    /*!
     * @brief Validates request body (JSON) against schema.
     *
     * @param request Request to be validated.
     *
     * @return Valid JSON object.
     * @throw ServerException if JSON is not valid.
     * */
    template<typename T>
    static json::Value validate_request_body(const rest::server::Request& request) {
        return validate_request_body(request, T::get_procedure());
    }


    /*!
     * @brief Validates request body (JSON) against schema.
     *
     * @param request Request to be validated.
     * @param schema ProcedureValidator schema against which validation is done.
     *
     * @return Valid JSON object.
     * @throw ServerException if JSON is not valid.
     * */
    static json::Value validate_request_body(const rest::server::Request& request,
                                             const jsonrpc::ProcedureValidator& schema);


    /*!
     * @brief Validates request body (JSON) against schema.
     *
     * @param request Request to be validated.
     * @param schema JSON schema against which validation is done.
     *
     * @return Valid JSON object
     * @throw ServerException if JSON is not valid.
     * */
    static json::Value validate_request_body(const rest::server::Request& request,
                                             const json::Value& schema) __attribute__ ((__deprecated__));


    /*!
     * @brief Validates JSON against schema.
     *
     * @param json JSON to be validated.
     * @param schema JSON schema against which validation is done.
     *
     * @return Valid JSON object
     * @throw ServerException if JSON is not valid.
     * */
    static json::Value validate_request_body(const json::Value& json,
                                             const json::Value& schema) __attribute__ ((__deprecated__));


    /*!
     * @brief Validates whether a request is empty.
     * This method accepts both a request with an empty body and an empty JSON object.
     *
     * @param request Request to be validated.
     *
     * @throw ServerException if request is not empty (as specified above).
     * */
    static void validate_empty_request(const rest::server::Request& request);


    /*!
     * @brief Validates whether a value is allowable agains a provided collection of values.
     *
     * @param[in] allowable_values Collection of allowable values of type T.
     * @param[in] value_to_check Value to be validated.
     *
     * @return True if value is allowed, false otherwise.
     * */
    template<typename T>
    static bool validate_allowable_values(const std::vector<T>& allowable_values, const T& value_to_check) {
        auto predicate = [&value_to_check](const T& allowable_value) {
            return allowable_value == value_to_check;
        };
        return std::any_of(allowable_values.begin(), allowable_values.end(), predicate);
    }


    /*!
     * @brief Gets json::Value representing 'any of' validator object.
     *
     * @param[in] type_to_validate Type to be validated.
     * @param[in] any_of_values JSON array containing 'any of' values.
     *
     * @return 'Any of' constraint JSON object.
     * */
    static json::Value any_of(const std::string& type_to_validate,
                              const json::Value& any_of_values) __attribute__ ((__deprecated__));


    /*!
     * @brief Gets json::Value representing 'type of' validator object.
     * @param type_to_validate Type to be validated.
     * @return JSON object with type constraints.
     * */
    static json::Value has_type(const std::string& type_to_validate) __attribute__ ((__deprecated__));


    /*!
     * @brief Gets json::Value representing 'more than' validator object.
     * @param[in] min_value The minimal value.
     * @return 'More than' constraint JSON object.
     * */
    static json::Value more_than(const double min_value) __attribute__ ((__deprecated__));


    /*!
     * @brief Sets JSON field as optional.
     *
     * @param[in] field_name The name of JSON field.
     * @param constraints Constraints generated by various
     * JsonValidator's methods.
     *
     * @return JSON Pair containing name and constraints of field.
     * */
    static json::Value optional(const std::string& field_name,
                                json::Value constraints) __attribute__ ((__deprecated__));


    /*!
     * @brief Sets JSON field as mandatory.
     *
     * @param[in] field_name The name of JSON field.
     * @param constraints Constraints generated by various
     * JsonValidator's methods.
     *
     * @return JSON Pair containing name and constraints of field.
     * */
    static json::Value mandatory(const std::string& field_name,
                                 json::Value constraints) __attribute__ ((__deprecated__));


    /*!
     * @brief Creates object containing constraints for JSON object
     *
     * @param[in] field_name The name of JSON field.
     * @param[in] object_value JSON object containing constraints for its fields.
     *
     * @return JSON object containing name and constraints of field.
     * */
    static json::Value object(const std::string& field_name,
                              const json::Value& object_value) __attribute__ ((__deprecated__));


private:
    static json::Value deserialize_json_from_request(const rest::server::Request& request);

};

}
}
}

