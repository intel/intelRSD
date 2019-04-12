/*!
 * @brief ValueRounder
 *
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
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
 * @file value_rounder.hpp
 */

#pragma once

#include "json-wrapper/json-wrapper.hpp"

namespace telemetry {

/*!
 * @brief Rounds value with respect to calculation precision
 */
template<double(*F)(double) = std::round>
class ValueRounder {
public:
    /*!
     * @brief Constructor
     * @param precision Calculation precision, should be greater than zero.
     */
    ValueRounder(double precision) : m_precision{precision} {
        if (m_precision <= 0) {
            throw std::runtime_error("precision should be greater then zero");
        }
    }

    /*!
     * @return Calculation precision used for rounding
     */
    double get_precision() const {
        return m_precision;
    }

    /*!
     * @brief Rounds value with respect to calculation precision
     * @param value value to be rounded
     * @return Rounded value
     */
    json::Json operator()(const json::Json& value) const {
        return (value.is_number()
                ? json::Json(F(value.get<double>() / m_precision) * m_precision)
                : value);
    }

private:
    double m_precision;
};

}
