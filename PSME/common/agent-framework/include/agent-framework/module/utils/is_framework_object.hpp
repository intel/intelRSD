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
 * @file is_framework_object.hpp
 *
 * @brief type_traits-like class to determine if something is a framework model
 * object with to_json, from_json methods
 * */

#pragma once
#include "json-wrapper/json-wrapper.hpp"

namespace agent_framework {
namespace model {
namespace utils {

/*! Type_traits-like class for determining if something is a framework class
 * */

template <typename T>
struct is_framework_object {
private:

    /* See is_framework_enum.h for explanation how it works */

    template <typename P>
    static constexpr bool check(P) {
        return false;
    }

    template <typename P, typename V = T>
    static constexpr auto check(P*)
            -> decltype(V::from_json(json::Json{}), true) {
        return true;
    }

public:

    // required by gcc to compile with no warnings
    is_framework_object() {}
    ~is_framework_object() {}

    /* @brief True if T is a framework model object, false otherwise. See
     * is_framework_enum.h for more info.
     * */
    static constexpr const bool value = check(static_cast<void*>(nullptr));
};

}
}
}
