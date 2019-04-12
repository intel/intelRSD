/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file port.hpp
 * @brief Definition of Fabric port class
 * */
#pragma once



#include "agent-framework/module/model/resource.hpp"
#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/enum/pnc.hpp"



namespace agent_framework {
namespace model {

/*! @brief Fabric port class */
class Port : public Resource {
public:

    using AllowedActions = attribute::Array<enums::ResetType>;
    using CablesIds = attribute::Array<std::string>;


    explicit Port(const std::string& parent_uuid = {}, enums::Component parent_type = enums::Component::Switch);


    virtual ~Port();


    /*! Enable copy */
    Port(const Port&) = default;


    Port& operator=(const Port&) = default;


    /*! Enable move */
    Port(Port&&) = default;


    Port& operator=(Port&&) = default;


    /*!
     * @brief Get collection name
     * @return collection name
     */
    static enums::CollectionName get_collection_name() {
        return Port::collection_name;
    }


    /*!
     * @brief Get component name
     * @return component name
     * */
    static enums::Component get_component() {
        return Port::component;
    }


    /*!
     * @brief Returns JSON representation of the object.
     * @return Json representation.
     * */
    json::Json to_json() const;


    /*!
     * @brief Constructs object from JSON
     * @param[in] json the json::Json deserialized to object
     * @return the newly constructed object
     */
    static Port from_json(const json::Json& json);


    /*!
     * @brief Set PortId
     * @param[in] port_id PortId
     * */
    void set_port_id(const OptionalField<std::string>& port_id) {
        m_port_id = port_id;
    }


    /*!
     * @brief Set Port Type structure
     * @param[in] port_type Port Type
     * */
    void set_port_type(const OptionalField<enums::PciePortType>& port_type) {
        m_port_type = port_type;
    }


    /*!
     * @brief Set cables ids structure
     * @param[in] cables_ids Cables ids
     * */
    void set_cables_ids(const CablesIds& cables_ids) {
        m_cables_ids = cables_ids;
    }


    /*!
     * @brief Set Link speed gbps
     * @param[in] speed_gbps Link speed gbps
     * */
    void set_speed_gbps(const OptionalField<double>& speed_gbps) {
        m_speed_gbps = speed_gbps;
    }


    /*!
     * @brief Set Link width
     * @param[in] width Link Width
     * */
    void set_width(const OptionalField<std::uint32_t>& width) {
        m_width = width;
    }


    /*!
     * @brief Set Max Link Speed Gbps
     * @param[in] max_speed_gbps MaxLinkSpeedGbps
     * */
    void set_max_speed_gbps(const OptionalField<double>& max_speed_gbps) {
        m_max_speed_gbps = max_speed_gbps;
    }


    /*!
     * @brief Set Max Link Width
     * @param[in] max_width Max Link Width
     * */
    void set_max_width(const OptionalField<std::uint32_t>& max_width) {
        m_max_width = max_width;
    }


    /*!
     * @brief Set TWI Port
     * @param[in] twi_port TWI Port
     * */
    void set_twi_port(uint32_t twi_port) {
        m_twi_port = twi_port;
    }


    /*!
     * @brief Set TWI Channel
     * @param[in] twi_channel TWI Channel
     * */
    void set_twi_channel(std::uint32_t twi_channel) {
        m_twi_channel = twi_channel;
    }


    /*!
     * @brief Get Port Id
     * @return Port Id
     * */
    const OptionalField<std::string>& get_port_id() const {
        return m_port_id;
    }


    /*!
     * @brief Set Physical PortId
     * @param[in] phys_port_id Physical PortId
     * */
    void set_phys_port_id(const uint32_t phys_port_id) {
        m_phys_port_id = phys_port_id;
    }


    /*!
     * @brief Get phys_port_id
     * @return Physical PortId
     * */
    uint32_t get_phys_port_id() const {
        return m_phys_port_id;
    }


    /*!
     * @brief Get Port Type
     * @return Port Type
     * */
    const OptionalField<enums::PciePortType>& get_port_type() const {
        return m_port_type;
    }


    /*!
     * @brief Get Cables Ids
     * @return Cables ids
     * */
    const CablesIds& get_cables_ids() const {
        return m_cables_ids;
    }


    /*!
     * @brief Get link speed gbps
     * @return Link speed gbps
     * */
    const OptionalField<double>& get_speed_gbps() const {
        return m_speed_gbps;
    }


    /*!
     * @brief Get link width
     * @return Link width
     * */
    const OptionalField<std::uint32_t>& get_width() const {
        return m_width;
    }


    /*!
     * @brief Get max link speed gbps
     * @return Max link speed gbps
     * */
    const OptionalField<double>& get_max_speed_gbps() const {
        return m_max_speed_gbps;
    }


    /*!
     * @brief Get max link width
     * @return Max link width
     * */
    const OptionalField<std::uint32_t>& get_max_width() const {
        return m_max_width;
    }


    /*!
     * @brief Get TWI Port
     * @return TWI Port Number
     * */
    std::uint32_t get_twi_port() const {
        return m_twi_port;
    }


    /*!
     * @brief Get TWI Channel
     * @return TWI Channel Number
     * */
    std::uint32_t get_twi_channel() const {
        return m_twi_channel;
    }


    /*!
     * @brief Returns list of all allowed actions on the port
     * @return List of allowed actions
     */
    const AllowedActions& get_allowed_actions() const {
        return m_allowed_actions;
    }


    /*!
     * @brief Sets list of all allowed actions on the port
     * @param allowed_actions List of allowed actions
     */
    void set_allowed_actions(const AllowedActions& allowed_actions) {
        m_allowed_actions = allowed_actions;
    }


    /*!
     * @brief Adds allowed action
     * @param allowed_action Allowed action
     */
    void add_allowed_action(const enums::ResetType& allowed_action) {
        m_allowed_actions.add_entry(allowed_action);
    }


    /*!
     * @brief Adds cable id
     * @param cable_id Cable id
     * */
    void add_cable_id(const std::string& cable_id) {
        m_cables_ids.add_entry(cable_id);
    }


    /*!
     * @brief Get protocol
     * @return protocol
     * */
    const OptionalField<enums::TransportProtocol>& get_protocol() const {
        return m_protocol;
    }


    /*!
     * @brief Set protocol
     * @param[in] protocol protocol
     * */
    void set_protocol(const OptionalField<enums::TransportProtocol>& protocol) {
        m_protocol = protocol;
    }


private:
    OptionalField<std::string> m_port_id{};
    OptionalField<enums::PciePortType> m_port_type{};
    OptionalField<double> m_speed_gbps{};
    OptionalField<std::uint32_t> m_width{};
    OptionalField<double> m_max_speed_gbps{};
    OptionalField<std::uint32_t> m_max_width{};
    AllowedActions m_allowed_actions{};
    CablesIds m_cables_ids{};
    OptionalField<enums::TransportProtocol> m_protocol{};

    // connection data used in the agent
    std::uint32_t m_twi_port{};
    std::uint32_t m_twi_channel{};
    // same as port_id but stored as non optional number
    std::uint32_t m_phys_port_id{};

    static const enums::CollectionName collection_name;
    static const enums::Component component;
};

}
}
