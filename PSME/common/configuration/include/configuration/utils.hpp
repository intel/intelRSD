/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2018 Intel Corporation
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
 * @section DESCRIPTION
 *
 * @file utils.hpp
 *
 * @brief Configuration utils
 * */

#pragma once
#include <string>

namespace json { class Value; }

namespace configuration {

/*!
* @brief Read file to JSON object
* @param filename Filename
* @param root JSON object
*/
void file_to_json(const std::string& filename, json::Value& root);

/*!
* @brief Write JSON object to file
* @param filename Filename
* @param root JSON object
*/
void json_to_file(const std::string& filename, json::Value& root);

/*!
 * @brief Convert JSON string to JSON object
 * @param json_string JSON string
 * @param root JSON object
 * @return On success true, otherwise false
 */
bool string_to_json(const std::string& json_string, json::Value& root);

/*!
 * @brief Convert JSON object to string
 * @param value JSON object
 * @return JSON string
 */
std::string json_value_to_string(const json::Value& value);

/*!
 * @brief Decrypt configuration string value
 * @param[in] key Decryption key (max 8 chars)
 * @param[in] message Hex encoded encrypted string to be decrypted
 * @return Decrypted string
 */
std::string decrypt(const std::string& key, const std::string& message);

}

