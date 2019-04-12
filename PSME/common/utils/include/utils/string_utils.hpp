/*!
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation.
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
 * @brief Utils for string operations
 * @file utils/string_utils.hpp
 */

#pragma once



#include <string>



namespace utils {

class StringUtils {
public:

    /*!
     * @brief Check if source string starts with target prefix
     * @param source Source text to check
     * @param target Prefix text
     * @return True if source string starts with target string
     */
    static bool starts_with(const std::string& source, const std::string& target);


    /*!
     * @brief Removes whitespaces from left end of a string.
     * @param input String to trim.
     * @return Trimmed string.
     */
    static std::string trim_left(std::string input);


    /*!
     * @brief Removes whitespaces from right end of a string.
     * @param input String to trim.
     * @return Trimmed string.
     */
    static std::string trim_right(std::string input);


    /*!
     * @brief Removes whitespaces from both ends of a string.
     * @param input String to trim.
     * @return Trimmed string.
     */
    static std::string trim(std::string input);


    /*!
     * @brief Get substring of the input after provided character
     * @param input Input string to be processed as substring.
     * @param character Character to be checked as last in the string
     * @return Substring of the input after provided character or empty string if character does not exist in the input.
     */
    static std::string substring_after_last_of(const std::string& input, char character);


    /*!
     * @brief Get empty string.
     * @return Empty string.
     */
    static const std::string& empty();

};

}
