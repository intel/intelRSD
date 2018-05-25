/*!
 * @header{License}
 * @copyright Copyright (c) 2017-2018 Intel Corporation.
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
 * @file jsonptr/include/jsonptr.hpp
 */
#include <stdexcept>
#include <string>
#include <vector>



namespace json {
class Value;
}

namespace jsonptr {

/*!
 * @brief Find fragment of a JSON document pointed at by a JSON pointer
 *
 * @param json JSON object to search through
 * @param jsonptr the pointer to fragment of JSON document
 * @return the fragment of JSON document pointed at by the pointer
 */
json::Value& find(const json::Value& json, const std::string& jsonptr);


/*!
 * @brief Set JSON fragment pointed at by a JSON pointer
 *
 * @param[in,out] json JSON object to modify
 * @param jsonptr the pointer to fragment of JSON document
 * @param value the value to be placed in the pointed part of the document
 */
void set_at(json::Value& json, const std::string& jsonptr, const json::Value& value);


/*!
 * Class representing a JSON pointer to be used with JSON-C++ library json values
 */
class JsonPtr {
public:
    static const char PTR_SEPARATOR;
    static const char ESCAPED_SEPARATOR[];
    static const char TILDE;
    static const char ESCAPED_TILDE[];


    /*!
     * @brief constructor of a JsonPtr object
     *
     * @param jsonptr string representation of a JSON pointer
     */
    JsonPtr(const std::string& jsonptr);


    /*!
     * @brief Find fragment of a JSON document pointed at by JsonPtr object
     *
     * @param json JSON object to search through
     * @return the fragment of JSON document pointed at by the pointer
     */
    json::Value& find(const json::Value& json);


    /*!
     * @brief Set JSON fragment pointed at by JsonPtr object
     *
     * @param[in,out] json JSON object to modify
     * @param value the value to be placed in the pointed part of the document
     */
    void set_at(json::Value& json, const json::Value& value);


    /*!
     * @brief escape special characters JSON pointer token according to RFC6901
     *
     * @param[in,out] segment JSON pointer segment
     */
    static void escape(std::string& segment);


    /*!
     * @brief unescape (restore) characters JSON pointer token according to RFC6901
     *
     * @param[in,out] escaped_segment JSON pointer segment
     */
    static void unescape(std::string& escaped_segment);

private:
    /*!
     * @brief replace all occurrences of a substring in string.
     * It's 2017 and still there's no such function in the standard library!
     *
     * @param[in,out] source the string that may need replacement of substrings
     * @param old substring to replace
     * @param replacement new substring
     */
    static void replace_all_occurrences(std::string& source, const std::string& old, const std::string& replacement);


    /*!
     * @brief parse JSON pointer segment as an JSON array index
     *
     * @param segment JSON pointer segment
     * @return the parsed index
     */
    unsigned long get_index(const std::string& segment);


    std::vector<std::string> m_segments;
};

/*!
 * @brief Class of errors thrown when JSON pointer is invalid and cannot be parsed
 */
class InvalidPointerError : public std::runtime_error {
public:
    /*!
     * @brief Constructors of exception object
     * @param what_arg Describes problem with JSON pointer
     */
    InvalidPointerError(const std::string& what_arg);
    InvalidPointerError(const InvalidPointerError&) = default;
    InvalidPointerError(InvalidPointerError&&) = default;

    /*!
     * @brief Destructor of exception object
     */
    virtual ~InvalidPointerError();
};

/*!
 * @brief Class of errors thrown when JSON pointer points to an unavailable fragment of a JSON document
 */
class NonexistentValueError : public std::runtime_error {
public:
    /*!
     * @brief Constructors of exception object
     * @param what_arg Describes problem with searching JSON document
     */
    NonexistentValueError(const std::string& what_arg);
    NonexistentValueError(const NonexistentValueError&) = default;
    NonexistentValueError(NonexistentValueError&&) = default;

    /*!
     * @brief Destructor of exception object
     */
    virtual ~NonexistentValueError();
};

}
