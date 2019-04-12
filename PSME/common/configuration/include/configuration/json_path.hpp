/*!
 * @section LICENSE
 *
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
 *
 * @section DESCRIPTION
 *
 * @file json_path.hpp
 *
 * @brief JsonPath interface
 * */

#pragma once
#include <deque>
#include <string>

namespace configuration {

/*! Collection of json keys */
class JsonPath {
public:
    explicit JsonPath() = default;

    /*!
     * @brief Add key to json path list
     * @param key json key
     */
    void push_key(const std::string& key);

    /*!
     * @brief Pop key from json path list
     */
    void pop_key();

    /*!
     * @brief Generate json path from current keys
     * @return Json path
     */
    std::string get_path();

private:
    /*! json path list */
    std::deque<std::string> m_keys{};
};

}
