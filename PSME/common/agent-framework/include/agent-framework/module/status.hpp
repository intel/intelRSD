/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * @file status.hpp
 * @brief Status
 * */

#ifndef AGENT_FRAMEWORK_MODULE_STATUS_HPP
#define AGENT_FRAMEWORK_MODULE_STATUS_HPP

#include <string>
#include <json/json.h>

namespace agent_framework {
namespace generic {

/*! Storage  status */
class Status {
    std::string m_state{};
    std::string m_health{};
public:
    /*! Default constructor */
    Status(){}

    /*!
     * @brief Assignement constructor for Status
     *
     * @param[in] state Status' state
     * @param[in] health Status' health
     * */
    Status(const std::string& state, const std::string& health) : m_state(state), m_health(health) {}

    /*! Default assignement operator */
    Status& operator=(const Status&) = default;

    /*! Copy constructor */
    Status(const Status&) = default;

    /*!
     * Sets Storage state
     *
     * @param[in] state Storage state
     * */
    void set_state(const std::string& state) { m_state = state; }

    /*!
     * Sets Storage health
     *
     * @param[in] health Storage health
     * */
    void set_health(const std::string& health) { m_health = health; }

    /*!
     * Gets Storage health
     * */
    const std::string& get_health() const { return m_health; }

    /*!
     * Gets Storage health
     * */
    const std::string& get_state() const { return m_state; }

    /*
     * Converts this object to json representation
     *
     * @return Json representation of this object
     * */
    Json::Value to_json() const;

    virtual ~Status();
};

}
}

#endif /* AGENT_FRAMEWORK_MODULE_STATUS_HPP */
