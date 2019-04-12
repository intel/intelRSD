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
 * */

#pragma once



#include "psme/rest/server/request.hpp"
#include "agent-framework/validators/procedure_validator.hpp"

#include <algorithm>

namespace psme {
namespace rest {
namespace validators {

class JsonValidator {
public:


    /*!
     * @brief Validates request body (JSON) against schema.
     *
     * @param request Request to be validated.
     *
     * @return Valid JSON object.
     * @throw ServerException if JSON is not valid.
     * */
    template<typename T>
    static json::Json validate_request_body(const rest::server::Request& request) {
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
    static json::Json validate_request_body(const rest::server::Request& request,
                                             const jsonrpc::ProcedureValidator& schema);

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


private:
    static json::Json deserialize_json_from_request(const rest::server::Request& request);

};

}
}
}

