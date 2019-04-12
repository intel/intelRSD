/*!
 * @brief Json optional class interface.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
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
 * @file json_optional.hpp
 */

#pragma once



#include "optional/optional-lib.hpp"
#include "json-wrapper/json-wrapper.hpp"



namespace spdk {

template<class T>
class JsonOptional : public std::experimental::optional<T> {

    template<class U>
    using optional = std::experimental::optional<U>;

public:

    JsonOptional() = default;


    /*!
     * @brief Converting constructor
     *
     * The template character of this converting constructor is needed so that
     * an JsonOptional object can be constructed from an object which is not of
     * the same type as JsonOptional template parameter (e.g.
     * OptionalValue<std::string> can be constructed from const char [])
     *
     * @param[in] in_value Value used to construct JsonOptional object
     */
    template<typename V>
    JsonOptional(const V& in_value) : optional<T>(T(in_value)) {}


    /*!
     * @brief Returns stored value if the object is present or default value of type T
     * @return Returns value of the optional or default value of type T
     */
    T value_or_default() const {
        return optional<T>::value_or(T{});
    }

};


/*!
 * @brief converts from JSON to Optional
 * @param json json object
 * @param optional Optional object
 */
template<class T>
void from_json(const json::Json& json, JsonOptional<T>& optional) {
    if (!json.is_null()) {
        optional = JsonOptional<T>(json.get<T>());
    }
    else {
        optional = JsonOptional<T>();
    }
}


/*!
 * @brief converts from Optional to Json
 * @param json Json object
 * @param optional Optional object
 */
template<class T>
void to_json(json::Json& json, const JsonOptional<T> optional) {
    if (optional) {
        json = optional.value();
    }
    else {
        json = nullptr;
    }
}

}