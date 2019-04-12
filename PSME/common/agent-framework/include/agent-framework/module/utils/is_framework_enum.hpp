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
 * @file is_framework_enum.hpp
 * @brief type_traits-like class to determine if something is a framework enum,
 * built using ENUM macro
 * */

#pragma once

#include <string>

namespace agent_framework {
namespace model {
namespace utils {

/*! Type_traits-like class for determining if something is a framework enum
 * */

template <typename T>
struct is_framework_enum {
private:

/*
 * How does it work:
 * -> For SFINAE to work we need to have compile-error in the declaration of one
 *    of the specializations to remove possible ambiguity. This is done via
 *    return type evaluation and "auto func() -> decltype(expression)" syntax.
 * -> We use comma operator "," in the decltype so that the rightmost decltype
 *    element will determine the deduced type (here it is bool basing on the
 *    'true' value in decltypes).
 * -> For type deduction to work we need these to be template functions.
 *    Therefore we need them be template functions of type V (with default
 *    type of T). This will cause expression T::from_string to be used for SFINAE.
 * -> Now, whatever we put in decltype() it will be used for deducing which
 *    template should be used. We want to have type_trait that is false unless
 *    class has a static method from_string. Therefore we need to have a general
 *    template returning false, and 'specialized' returning true.
 * -> We have to use a trick to evade ambiguity problem - specialized template
 *    has to be a better fit than the generalized template.
 * -> We achieve it by adding a dummy parameter of the function. If this template
 *    is used with a pointer -> specialized version will be a better fit, therefore
 *    as long as V::from_string is present, specialized version is used.
 * -> In case V::from_string does not exist, specialized version is an ill-formed
 *    code and due to SFINAE, generalized version is used.
 * -> While using this code, we need to pass a dummy parameter which is a pointer.
 *    Here we use static_cast<void*>(nullptr).
 */

    template <typename P>
    static constexpr bool check(P) {
       return false;
    }

    template <typename P, typename V = T>
    static constexpr auto check(P*)
            -> decltype(V::from_string(std::string{}), true) {
        return true;
    }

public:

    // required by gcc to compile with no warnings
    is_framework_enum() {}
    ~is_framework_enum() {}

    /* @brief This variable will be equal to true if T is a framework enum
     * or false if T is an object model. Distinction is made based on the
     * presence of from_json or from_string member function. This can be used
     * in the same way as any is_* (e.g. is_class) template from the type_trait
     * header.
     * */
    static constexpr const bool value = check(static_cast<void*>(nullptr));

};

}
}
}
