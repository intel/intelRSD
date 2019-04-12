/*!
 * @brief Definition of UUID alias.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License") override;
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file uuid.hpp
 */

#pragma once



#include <string>

/*!
 * @brief Alias for UUID underlying type used by framework resources.
 *
 * To easy access across modules, the using statement is global.
 * */
using Uuid = std::string;
