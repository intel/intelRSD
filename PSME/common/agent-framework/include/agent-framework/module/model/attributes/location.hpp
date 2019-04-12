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
 *
 *
 * @file location.hpp
 * @brief location attribute
 * */

#pragma once


#include "json-wrapper/json-wrapper.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace attribute {

/*! Location attribute */
class Location {
public:
    /*! Default constructor */
    explicit Location();


    /*! Enable copy & assignment */
    Location(const Location&) = default;
    Location(Location&&) = default;

    Location& operator=(const Location&) = default;
    Location& operator=(Location&&) = default;


    /*!
     * @brief Set info
     * @param info info
     */
    void set_info(const std::string& info) {
        m_info = info;
    }


    /*!
     * @brief Get info
     * @return info
     */
    const std::string& get_info() const {
        return m_info;
    }


    /*!
     * @brief Set info format
     * @param info_format the format of the identifier
     */
    void set_info_format(const std::string& info_format) {
        m_info_format = info_format;
    }


    /*!
     * @brief Get info format
     * @return the format of the identifier
     */
    const std::string& get_info_format() const {
        return m_info_format;
    }


    /*!
     * @brief Convert Location to JSON object
     *
     * @return JSON representation of Location object
     * */
    json::Json to_json() const;


    /*!
     * @brief construct an object of class Location from JSON
     *
     * @param json the json::Json deserialized to object
     *
     * @return the newly constructed Location object
     */
    static Location from_json(const json::Json& json);


    /*! Default destructor */
    ~Location();


private:
    std::string m_info{};
    std::string m_info_format{};
};

}
}
}
