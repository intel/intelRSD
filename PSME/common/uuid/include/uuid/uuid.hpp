/*!
 * @brief declaration of wrapper functions for OSSP UUID library
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
 * @file common/uuid/include/uuid/uuid.hpp
 */

#include <string>

/*!
 * @brief generate a version 1 (temporary or random in our jargon), date-time and MAC-based UUID
 *
 * @return the generated uuid
 */
std::string make_v1_uuid();


/*!
 * @brief generate a version 5 (persistent in our jargon), UUID based on SHA1 hash
 * @param namespace_uuid the namespace identifier to hash
 * @param unique_name the name to hash
 * @return the generated uuid
 */
std::string make_v5_uuid(const std::string& namespace_uuid, const std::string& unique_name);


/*!
 * @brief unparse uuid string from guid
 * @param guid_ptr Pointer to guid object
 * @return unparse uuid
 */
std::string unparse_uuid(const void* guid_ptr);


/*!
 * @brief check if given string uuid has valid format ffffffff-ffff-ffff-ffff-ffffffffffff
 * @param uuid string uuid
 * @return true if uuid is valid, false otherwise
 */
bool is_valid_string_uuid(const std::string& uuid);