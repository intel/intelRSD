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
 *
 *
 * @file json_to_string_vector.cpp
 * @brief json to vector<string> converting function
 * */

#include "agent-framework/module-ref/utils/optional/extensions.hpp"

#include <cmath>
#include <limits>

namespace std {
namespace experimental {

/* Template specialization for safe FP numbers comparison */
template<>
bool operator==(const std::experimental::optional<double>& lhs, const std::experimental::optional<double>& rhs) {
    if(!lhs || !rhs) {
        return false;
    }
    return std::abs(lhs.value() - rhs.value()) < std::numeric_limits<double>::epsilon();
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
    if(!rhs) {
        return false;
    }
    return std::abs(lhs - rhs.value()) < std::numeric_limits<double>::epsilon();
}

template<>
bool operator!=(const std::experimental::optional<double>& lhs, const std::experimental::optional<double>& rhs) {
    if(!lhs || !rhs) {
        return true;
    }
    return std::abs(lhs.value() - rhs.value()) > std::numeric_limits<double>::epsilon();
}

template<>
bool operator!=(const std::experimental::optional<double>& lhs, const double& rhs) {
    if(!lhs) {
        return true;
    }
    return std::abs(lhs.value() - rhs) > std::numeric_limits<double>::epsilon();
}

template<>
bool operator!=(const double& lhs, const std::experimental::optional<double>& rhs) {
    if(!rhs) {
        return true;
    }
    return std::abs(lhs - rhs.value()) > std::numeric_limits<double>::epsilon();
}

/* Logical operators for safe string comparison */
bool operator==(const std::experimental::optional<std::string>& lhs, const std::experimental::optional<std::string>& rhs) {
    return (bool(lhs) && bool(rhs) ? lhs.value()==rhs.value() : false );
}

bool operator==(const char* lhs, const std::experimental::optional<std::string>& rhs) {
    return ( bool(rhs) ? lhs==rhs.value() : false );
}

bool operator==(const std::experimental::optional<std::string>& lhs, const char* rhs) {
    return ( bool(lhs) ? lhs.value()==rhs : false );
}

bool operator!=(const std::experimental::optional<std::string>& lhs, const std::experimental::optional<std::string>& rhs) {
    return lhs.value()!=rhs.value();
}

bool operator!=(const char* lhs, const std::experimental::optional<std::string>& rhs) {
    return lhs!=rhs.value();
}

bool operator!=(const std::experimental::optional<std::string>& lhs, const char* rhs) {
    return lhs.value()!=rhs;
}

}
}
