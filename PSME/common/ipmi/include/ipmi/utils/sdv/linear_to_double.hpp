/*!
 * @brief Function converting linear format to double
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
 * @file ipmi/utils/sdv/linear_to_double.hpp
 */

#pragma once

namespace ipmi {
namespace sdv {

/*!
 * @brief Converts linear format data into double
 * @param linear Value stored in linear format
 * @return Input data converted to double format
 */
double linear_to_double(std::uint16_t linear);

}
}
