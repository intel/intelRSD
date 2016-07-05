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
 * @file neighbor_info.hpp
 * @brief Neighbor info
 * */
#ifndef AGENT_FRAMEWORK_MODULE_NEIGHBOR_INFO_HPP
#define AGENT_FRAMEWORK_MODULE_NEIGHBOR_INFO_HPP

#include <jsonrpccpp/server.h>

#include <string>

namespace agent_framework {
namespace generic {

/*! Neighbor info */
class NeighborInfo {
    std::string m_switch_identifier{};
    std::string m_port_identifier{};
public:
    /*!
     * @brief NeighborInfo constructor
     *
     * @param[in] switch_identifier Switch identifier
     * @param[in] port_identifier Port identifier
     * */
    NeighborInfo(const std::string& switch_identifier = {},
                 const std::string& port_identifier = {}) :
            m_switch_identifier(switch_identifier),
            m_port_identifier(port_identifier) {}

    /*! Default assignement operator */
    NeighborInfo& operator=(const NeighborInfo&) = default;

    /* Default copy constructor */
    NeighborInfo(const NeighborInfo&) = default;

    /*!
     * @brief Set switch identifier
     * @param[in] switch_identifier Switch identifier
     * */
    void set_switch_identifier(const std::string& switch_identifier) {
        m_switch_identifier = switch_identifier;
    }

    /*!
     * Gets switch identifier.
     * */
    const std::string& get_switch_identifier() const {
        return m_switch_identifier;
    }

    /*!
     * @brief Set port identifier
     * @param[in] port_identifier Port identifier
     * */
    void set_port_identifier(const std::string& port_identifier) {
        m_port_identifier = port_identifier;
    }

    /*!
     * Gets port identifier.
     * */
    const std::string& get_port_identifier() const {
        return m_port_identifier;
    }

    /*!
     * @brief Convert NeighborInfo to JSON object
     * @return JSON representation of NeighborInfo object
     * */
    Json::Value to_json() const;

    virtual ~NeighborInfo();
};

}
}

#endif /* AGENT_FRAMEWORK_MODULE_NEIGHBOR_INFO_HPP */
