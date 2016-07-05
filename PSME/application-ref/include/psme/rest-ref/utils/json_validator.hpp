/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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

#ifndef PSME_UTILS_JSON_VALIDATOR_HPP
#define	PSME_UTILS_JSON_VALIDATOR_HPP
#include "psme/rest-ref/server/request.hpp"

/* Forward declaration */
namespace json {
    class Value;
}

namespace psme {
namespace rest {
namespace utils {

class JsonValidator {
public:
    static constexpr const char STRING_TYPE[] = "string";
    static constexpr const char ARRAY_TYPE[] = "array";
    static constexpr const char DOUBLE_TYPE[] = "double";
    static constexpr const char BOOL_TYPE[] = "bool";
    static constexpr const char UINT_TYPE[] = "uint";
    static constexpr const char OBJECT_TYPE[] = "object";
    static constexpr const char NUMBER_TYPE[] = "number";
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
                                             const json::Value& schema);

    /*!
     * @brief Validates JSON against schema.
     *
     * @param request JSON to be validated.
     * @param schema JSON schema against which validation is done.
     *
     * @return Valid JSON object
     * @throw ServerException if JSON is not valid.
     * */
    static json::Value validate_request_body(const json::Value& json,
                                             const json::Value& schema);

    /*!
     * @brief Gets json::Value representing 'any of' validator object.
     *
     * @param[in] type_to_validate Type to be validated.
     * @param[in] any_of_values JSON array containing 'any of' values.
     * @param mandatory Indicate mandatory validation.
     *
     * @return 'Any of' JSON object.
     * */
    static json::Value any_of(const std::string& type_to_validate,
                              const json::Value& any_of_values,
                              bool mandatory = false);

     /*!
     * @brief Gets json::Value representing 'type of' validator object.
     *
     * @param type_to_validate Type to be validated.
     * @param mandatory Indicate mandatory validation.
     *
      * @return 'Type of' JSON object.
     * */
    static json::Value type_of(const std::string& type_to_validate,
                               bool mandatory = false);

    /*!
     * @brief Gets json::Value representing 'more than' validator object.
     *
     * @param[in] min_value the minimal value.
     * @param mandatory Indicate mandatory validation.
     *
     * @return 'More than' JSON object.
     * */
    static json::Value more_than(const double min_value,
                                 bool mandatory = false);


};
}
}
}

#endif	/* PSME_UTILS_JSON_VALIDATOR_HPP */
