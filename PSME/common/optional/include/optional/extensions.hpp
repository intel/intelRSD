/*!
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
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
 * @brief Extensions for optional functionality.
 * */

#pragma once
#include "optional-lib.hpp"

namespace std {
namespace experimental {

/* Functions for safe floating-point number comparison */
/*!
 * @brief Template specialization for safe floating-point number comparison.
 *
 * Extends the functionality defined by Boost library.
 *
 * @param[in] lhs Object containing a FP number on the LHS of the operator
 * @param[in] rhs Object containing a FP number on the LHS of the operator
 *
 * @return[] True if values held by std::optional object differ by less than the
 * smallest FP number representible on the machine as defined by std::limits.
 * */
template<>
bool operator==(const std::experimental::optional<double>& lhs, const std::experimental::optional<double>& rhs);

/*!
 * @brief Template specialization for safe floating-point number comparison.
 *
 * Extends the functionality defined by Boost library.
 *
 * @param[in] lhs Object containing a FP number on the LHS of the operator
 * @param[in] rhs FP number on the RHS of the operator
 *
 * @return[] True if value held by std::optional and RHS differ by less than the
 * smallest FP number representible on the machine as defined by std::limits.
 * */
template<>
bool operator==(const std::experimental::optional<double>& lhs, const double& rhs);

/*!
 * @brief Template specialization for safe floating-point number comparison.
 *
 * Extends the functionality defined by Boost library.
 *
 * @param[in] lhs FP number on the LHS of the operator
 * @param[in] rhs Object containing a FP number on the RHS of the operator
 *
 * @return[] True if value held by std::optional and LHS differ by less than the
 * smallest FP number representible on the machine as defined by std::limits.
 * */
template<>
bool operator==(const double& lhs, const std::experimental::optional<double>& rhs);

/*!
 * @brief Template specialization for safe floating-point number comparison.
 *
 * Extends the functionality defined by Boost library.
 *
 * @param[in] lhs Object containing a FP number on the LHS of the operator
 * @param[in] rhs Object containing a FP number on the RHS of the operator
 *
 * @return[] True if values held by std::optional object differ by more than the
 * smallest FP number representible on the machine as defined by std::limits.
 * */
template<>
bool operator!=(const std::experimental::optional<double>& lhs, const std::experimental::optional<double>& rhs);

/*!
 * @brief Template specialization for safe floating-point number comparison.
 *
 * Extends the functionality defined by Boost library.
 *
 * @param[in] lhs Object containing a FP number on the LHS of the operator
 * @param[in] rhs FP number on the RHS of the operator
 *
 * @return[] True if value held by std::optional and RHS differ by more than the
 * smallest FP number representible on the machine as defined by std::limits.
 * */
template<>
bool operator!=(const std::experimental::optional<double>& lhs, const double& rhs);

/*!
 * @brief Template specialization for safe floating-point number comparison.
 *
 * Extends the functionality defined by Boost library.
 *
 * @param[in] lhs FP number on the LHS of the operator
 * @param[in] rhs Object containing a FP number on the RHS of the operator
 *
 * @return[] True if value held by std::optional and RHS differ by more than the
 * smallest FP number representible on the machine as defined by std::limits.
 * */
template<>
bool operator!=(const double& lhs, const std::experimental::optional<double>& rhs);

/*!
 * @brief Comparison operator for safe std::experimental::optional<std::string> objects comparison
 *
 * Extends the functionality defined by Boost library.
 *
 * @param[in] lhs Object on the LHS of the comparison operator
 * @param[in] rhs Object on the RHS of the comparison operator
 *
 * @return True if both objects containt the same string, false otherwise.
 * */
bool operator==(const optional<std::string>& lhs, const char* rhs);

/*!
 * @brief Comparison operator for safe std::experimental::optional<std::string> objects comparison
 *
 * Extends the functionality defined by Boost library.
 *
 * @param[in] lhs Object on the LHS of the comparison operator
 * @param[in] rhs Object on the RHS of the comparison operator
 *
 * @return True if both objects containt the same string, false otherwise.
 * */
bool operator==(const char* lhs, const optional<std::string>& rhs);

/*!
 * @brief Comparison operator for safe std::experimental::optional<std::string> objects comparison
 *
 * Extends the functionality defined by Boost library.
 *
 * @param[in] lhs Object on the LHS of the comparison operator
 * @param[in] rhs Object on the RHS of the comparison operator
 *
 * @return True if both objects containt the same string, false otherwise.
 * */
bool operator!=(const optional<std::string>& lhs, const char* rhs);

/*!
 * @brief Comparison operator for safe std::experimental::optional<std::string> objects comparison
 *
 * Extends the functionality defined by Boost library.
 *
 * @param[in] lhs Object on the LHS of the comparison operator
 * @param[in] rhs Object on the RHS of the comparison operator
 *
 * @return True if both objects containt the same string, false otherwise.
 * */
bool operator!=(const char* lhs, const optional<std::string>& rhs);

}
}

