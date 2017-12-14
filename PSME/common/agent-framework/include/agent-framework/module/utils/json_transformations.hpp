/*!
 * @header{License}
 * @copyright Copyright (c) 2017 Intel Corporation.
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
 * @header{Filesystem}
 * @file json_transformations.hpp
 */

#pragma once

#include "json-wrapper/json-wrapper.hpp"
#include "json/json.hpp"

namespace agent_framework {
namespace model {
namespace utils {

/*!
 * @brief Convert json::Value from JSON-C++ to JsonCpp value
 * @param src JSON to be converted
 * @deprecated Utility to be used for converting JSON-C++ to JsonCpp ones
 */
json::Json to_json_rpc(const json::Value& src) __attribute__((deprecated));


/*!
 * @brief Convert json::Json from JsonCpp to JSON-C++ value
 * @param src JSON to be converted
 * @deprecated Utility to be used for converting JsonCpp to JSON-C++ ones
 */
json::Value to_json_cxx(const json::Json& src) __attribute__((deprecated));

}
}
}
