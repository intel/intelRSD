/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * @brief Arrays template used in commands
 * */

#pragma once
#include <json/json.h>

#include <vector>
#include <string>
#include <type_traits>

using std::string;

namespace Json {
    /* Forward declaration */
    class Value;
}

namespace agent_framework {
namespace model {
namespace attribute {

/*! Class representing arrays in commands response
 * */

template <typename T>
class Array {
private:
    using ArrayObject = std::vector<T>;

public:
    explicit Array();

    /*!
     * @brief Constructor from vector
     * @param[in] v vector<T>
     */
    Array(const ArrayObject& v) : m_array(v) { }

    /*!
     * @brief Constructor from initializer list
     * @param[in] list initializer list
     */
    Array(const std::initializer_list<T> list) : m_array(list) { }

    /*! @brief Destructor */
    ~Array();

    /*! Enable copy */
    Array(const Array&) = default;
    Array& operator=(const Array&) = default;

    /*! Enable move */
    Array(Array&&) = default;
    Array& operator=(Array&&) = default;

    /*!
     * @brief Sets array
     * @param array Array
     */
    void set_array(const ArrayObject& array) {
        m_array = array;
    }

    /*!
     * @brief Returns array
     * @return Array
     */
    const ArrayObject& get_array() const {
        return m_array;
    }

    /*!
     * @brief Adds entry to the array
     * @param entry Entry to be added
     */
    void add_entry(const T& entry) {
        m_array.emplace_back(entry);
    }

     /*!
      * @brief Creates array from the json object
      * @param json Json object
      * @return Array
      */
    static Array<T> from_json(const Json::Value& json);

    /*!
     * @brief Converts array into json object
     * @return Json object
     */
    Json::Value to_json() const;

    /*!
     * @brief Creates a subarray consisting to this array's
     * elements between the indexes given
     * @param[in] first first index to copy to subarray
     * @param[in] last last index to copy to subarray
     * @return Array subarray
     */
    Array<T> subarray(uint first, uint last) const {
        return Array<T>(ArrayObject(&m_array[first], &m_array[last]));
    }

    /*!
     * @brief Returns iterator to the first element of the array
     * @return iterator
     */
    typename ArrayObject::iterator begin() {
        return m_array.begin();
    }

    /*!
     * @brief Returns iterator to the element after the last element
     * @return iterator
     */
    typename ArrayObject::iterator end() {
        return m_array.end();
    }

    /*!
     * @brief Returns const iterator to the first element of the array
     * @return iterator
     */
    typename ArrayObject::const_iterator begin() const {
        return m_array.begin();
    }

    /*!
     * @brief Returns const iterator to the element after the last element
     * @return iterator
     */
    typename ArrayObject::const_iterator end() const {
        return m_array.end();
    }

    /*!
     * @brief Checks if the array is empty.
     * @return Returns true if array is empty, false otherwise.
     * */
    bool empty() const {
        return m_array.empty();
    }

    /*!
     * @brief Element access [] operator
     * @param[in] index Index of element
     * @return Reference to the element
     * */
    T& operator[](size_t index) {
        return m_array[index];
    }

    /*!
     * @brief Element access [] operator (const version)
     * @param[in] index Index of element
     * @return Const reference to the element
     * */
    const T& operator[](size_t index) const {
        return m_array[index];
    }

    /*!
     * @brief Returns size of the array
     * @return Size of the array
     * */
    size_t size() const {
        return m_array.size();
    }

private:
    ArrayObject m_array{};
};

}
}
}
