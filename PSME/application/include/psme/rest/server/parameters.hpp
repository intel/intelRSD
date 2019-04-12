/*
 * Copyright (C) 2014 MediaSift Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 * */

#pragma once

#include <unordered_map>
#include <string>

namespace psme {
namespace rest {
namespace server {

/*!
 * @brief Represents a collection of named parameters passed via the request URI.
 *
 * This class represents named parameters defined by the user when registering
 * method handlers in a multiplexer.
 *
 * When the multiplexer matches a method handler, the parsed parameters are
 * stored in an instance of this object which is then passed to the handler via
 * the request object.
 *
 * Parameters are stored in a hash map.
 * */
class Parameters {
public:
    using ParameterList = std::unordered_map<std::string, std::string>;
    using ValueType = typename ParameterList::value_type;

    /*!
     * @brief Accesses a parameter based on a key.
     *
     * The reference to the parameter is returned
     * and can be modified through this operator. If the
     * key does not yet exist then it is created.
     *
     * @param key the key of the parameter
     * @return the parameter stored under the key
     * */
    std::string& operator[](const std::string& key);

    /*!
     * @brief Set a parameter.
     *
     * @param key the key to store the parameter under
     * @param value the value of the parameter
     * */
    void set(const std::string& key, const std::string& value);


    /*!
     * @brief Obtain the value of a parameter.
     *
     * @param key the key of the parameter
     *
     * @return The value of the parameter, or an empty string if the key is not matched.
     * */
    std::string operator[](const std::string& key) const;

    /*!
     * @brief Obtain the value of a parameter.
     *
     * @param key the key of the parameter
     *
     * @return The value of the parameter, or an empty string if the key is not matched.
     * */
    std::string get(const std::string& key) const;

    /*!
     * @brief Returns size of parameter list
     * @return Size of parameter list
     */
    unsigned long size() const;

    ParameterList::iterator begin() {
        return m_list.begin();
    }

    ParameterList::iterator end() {
        return m_list.end();
    }

    ParameterList::const_iterator begin() const {
        return m_list.begin();
    }

    ParameterList::const_iterator end() const {
        return m_list.end();
    }

private:
    ParameterList m_list{};
};

}
}
}

