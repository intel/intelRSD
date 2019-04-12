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
 * @file neighbor_info.hpp
 * @brief Neighbor info
 * */
#pragma once
#include "agent-framework/module/utils/utils.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace attribute {

/*! Neighbor info */
class NeighborInfo {
public:

    explicit NeighborInfo();

    NeighborInfo(const std::string& switch_identifier,
                 const std::string& port_identifier,
                 const std::string& cable_id):
                 m_switch_identifier(switch_identifier),
                 m_port_identifier(port_identifier),
                 m_cable_id(cable_id)
                 {}

    ~NeighborInfo();

    /*! Enable copy */
    NeighborInfo(const NeighborInfo&) = default;
    NeighborInfo& operator=(const NeighborInfo&) = default;
    NeighborInfo(NeighborInfo&&) = default;
    NeighborInfo& operator=(NeighborInfo&&) = default;

    /*!
     * @brief Set switch identifier
     * @param[in] switch_identifier Switch identifier
     * */
    void set_switch_identifier(const OptionalField<std::string>& switch_identifier) {
        m_switch_identifier = switch_identifier;
    }

    /*!
     * Gets switch identifier.
     * */
    const OptionalField<std::string>& get_switch_identifier() const {
        return m_switch_identifier;
    }

    /*!
     * @brief Set port identifier
     * @param[in] port_identifier Port identifier
     * */
    void set_port_identifier(const OptionalField<std::string>& port_identifier) {
        m_port_identifier = port_identifier;
    }

    /*!
     * Gets port identifier.
     * */
    const OptionalField<std::string>& get_port_identifier() const {
        return m_port_identifier;
    }

    /*!
     * @brief Set cable id
     * @param[in] cable_id Cable ID
     * */
    void set_cable_id(const OptionalField<std::string>& cable_id) {
        m_cable_id = cable_id;
    }

    /*!
     * Gets cable ID.
     * */
    const OptionalField<std::string>& get_cable_id() const {
        return m_cable_id;
    }

    /*!
     * @brief Convert NeighborInfo to JSON object
     * @return JSON representation of NeighborInfo object
     * */
    json::Json to_json() const;

    /*!
     * @brief construct an object of class NeighborInfo from JSON
     *
     * @param json the json::Json deserialized to object
     *
     * @return the newly constructed NeighborInfo object
     */
    static NeighborInfo from_json(const json::Json& json);

private:

    OptionalField<std::string> m_switch_identifier{};
    OptionalField<std::string> m_port_identifier{};
    OptionalField<std::string> m_cable_id{};

};

}
}
}
