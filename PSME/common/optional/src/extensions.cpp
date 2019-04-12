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
 * @file extensions.cpp
 * @brief Optional extensions
 * */

#include "optional/extensions.hpp"

#include <cmath>
#include <limits>

namespace std {
namespace experimental {

/* Template specialization for safe FP numbers comparison */
template<>
bool operator==(const std::experimental::optional<double>& lhs, const std::experimental::optional<double>& rhs) {
    return (bool(lhs) != bool(rhs)) ? false : !bool(rhs) ? true
               : std::abs(*lhs - *rhs) < std::numeric_limits<double>::epsilon();
}

template<>
bool operator==(const std::experimental::optional<double>& lhs, const double& rhs) {
    if(!lhs) {
        return false;
    }
    return std::abs(lhs.value() - rhs) < std::numeric_limits<double>::epsilon();
}

template<>
bool operator==(const double& lhs, const std::experimental::optional<double>& rhs) {
    /* forward to operator==(optional, double) */
    return (rhs == lhs);
}

template<>
bool operator!=(const std::experimental::optional<double>& lhs, const std::experimental::optional<double>& rhs) {
    return !(lhs == rhs);
}

template<>
bool operator!=(const std::experimental::optional<double>& lhs, const double& rhs) {
    return !(lhs == rhs);
}

template<>
bool operator!=(const double& lhs, const std::experimental::optional<double>& rhs) {
    return !(lhs == rhs);
}

/* Logical operators for safe string comparison */
bool operator==(const std::experimental::optional<std::string>& lhs, const char* rhs) {
    return (bool(lhs) != (rhs != nullptr)) ? false : bool(lhs) ? *lhs == rhs : true;
}

bool operator==(const char* lhs, const std::experimental::optional<std::string>& rhs) {
    /* forward to operator==(optional, char*) */
    return (rhs == lhs);
}

bool operator!=(const char* lhs, const std::experimental::optional<std::string>& rhs) {
    return !(lhs == rhs);
}

bool operator!=(const std::experimental::optional<std::string>& lhs, const char* rhs) {
    return !(lhs == rhs);
}

}
}
