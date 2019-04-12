/*!
 * @brief Switch Port Info class declaration.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
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
 * @file switch_port_info.hpp
 */

#pragma once

#include "agent-framework/module/enum/network.hpp"

#include <cstdint>
#include <vector>
#include <string>
#include <map>

namespace agent {
namespace network {
namespace hal {

/*!
 * @brief Switch port info. Need to set switch id and port number.
 */
class SwitchPortInfo {
public:

    using PortId = std::uint8_t;
    using VlanId = std::uint16_t;
    using SwitchId = std::uint8_t;
    using PortIdentifier = std::string;
    using State = agent_framework::model::enums::OperationalState;
    using PublicVlans = std::vector<std::uint32_t>;
    using PfcEnabledPriorities = std::vector<std::uint32_t>;

    /*! @brief Port attributes. */
    enum PortAttributeType {
        STATUS, //!< Port Status
        PORTIDENTIFIER, //!< Port Identifier
        PORTTYPE, //!< Port type
        PORTMODE, //!< Port mode
        LINKTECHNOLOGY, //!< This is link technology, such as Ethernet, for this NIC
        LINKSPEEDMBPS, //!< Current port speed
        MAXSPEEDMBPS, //!< Max port speed
        ADMINISTRATIVESTATE, //!< Administrative state
        OPERATIONALSTATE, //!< Operational (administrative) state
        PORTWIDTH, //!< Port width, for PCI-e port this is number of PCI-e lanes
        FRAMESIZE, //!< MAC Frame size in bytes
        AUTOSENSE, //!< Indicates if the speed and duplex is automatically configured by the port
        ISMANAGEMENTPORT, //!< Indicates if a port may be used for switch management
        LASTERRORCODE, //!< Code of last error detected
        ERRORCLEARED, //!< Error cleared
        LASTSTATECHANGETIME, //!< Time of the last port state change
        MACADDRESS, //!< Switch port MAC address
        IPV4ADDRESS, //!< IPv4 address
        IPV4SUBNETMASK, //!< IPv4 subnet mask
        IPV6ADDRESS, //!< IPv6 address
        NEIGHBORINFO, //!< Neighbor information
        NEIGHBORMAC, //!< Neighbor MAC
        VLANENABLE, //!< Indicates if VLANs are enabled on the switch port
        VLANLIST, //!< List of created VLANs
        DEFAULTVID, //!< Default VLAN Id
        PFC_ENABLED, //!< Priority Flow Control Enabled
        PFC_ENABLED_PRIORITIES, //!< Priority Flow Control No-Drop Enabled Priorities
        LLDP_ENABLED, //!< Link Layer Discovery Protocol Enabled
        DCBX_STATE //!< Data Center Bridging Capability Exchange Protocol mode
    };

    /*! Port attribute value class */
    class PortAttributeValue {
    public:
        /*!
         * @brief Default constructor.
         * */
        PortAttributeValue() = default;

        /*!
         * @brief Default destructor.
         * */
        ~PortAttributeValue() { }

        /*!
         * @brief Get bool value
         *
         * @return Bool value.
         * */
        bool get_bool() const;

        /*!
         * @brief Get number value
         *
         * @return Number value.
         * */
        std::uint32_t get_number() const;

        /*!
         * @brief Get string value
         *
         * @return String value.
         * */
        const std::string& get_string() const;

        /*!
         * @brief Set bool value
         *
         * @param[in] value Value to set.
         * */
        void set(bool value);

        /*!
         * @brief Set number value
         *
         * @param[in] value Value to set.
         * */
        void set(std::uint32_t value);

        /*!
         * @brief Set string value
         *
         * @param[in] value Value to set.
         * */
        void set(const std::string& value);

        /*!
         * @brief Set value based on JSON object
         *
         * @param[in] value Value to set.
         * */
        void set_value_from_json(const json::Json& value);

        /*!
         * @brief Checks if value was set
         *
         * @return true if value was set, false otherwise.
         * */
        bool is_set() { return NOTSET != m_type; }


        /*! Explicit class conversation to int */
        explicit operator int() const { return int(get_number()); }

        /*! Explicit class conversation to uint32_t */
        explicit operator std::uint32_t() const { return get_number(); }

        /*! Explicit class conversation to bool */
        explicit operator bool() const { return get_bool(); }

        /*! Explicit class conversation to string */
        explicit operator std::string() const { return get_string(); }

    private:
        enum ValueType {
            NOTSET,
            NUMBER,
            BOOL,
            STRING
        };
        ValueType m_type{NOTSET};
        std::uint32_t m_number{};
        bool m_bool{false};
        std::string m_string{};
    };

    using VlanList = std::vector<std::string>;
    using PortList = std::vector<std::string>;

    /*!
     * @enum Duplex
     * @brief Duplex mode.
     *
     * @var Duplex::HALF
     * half duplex
     *
     * @var Duplex::FULL
     * full duplex
     *
     * @var Duplex::UNKNOWN
     * unknown duplex
     * */
    enum class Duplex {
        HALF = 0,
        FULL,
        UNKNOWN
    };

    /*! Vlan information */
    class VlanInfo {
    public:
        /*! @brief Default constructor */
        VlanInfo() = delete;

        /*!
         * @brief Default constructor.
         *
         * @param[in] vlan_id Vlan id.
         * @param[in] tag Vlan tag.
         * */
        VlanInfo(std::uint32_t vlan_id, bool tag) :
                    m_vlan_id{vlan_id}, m_tag{tag} { }

        /*!
         * @brief Get vlan id.
         *
         * @return Vlan id.
         * */
        std::uint32_t get_vlan_id() const {
            return m_vlan_id;
        }

        /*!
         * @brief Check if is tagged.
         *
         * @return true if tagged, false otherwise.
         * */
        bool is_tagged() const {
            return m_tag;
        }

    private:
        std::uint32_t m_vlan_id{};
        bool m_tag{};
    };

    using VlanInfoList = std::vector<VlanInfo>;

    /*!
     * @brief Default constructor.
     * */
    SwitchPortInfo() = default;

    /*!
     * @brief Default constructor.
     *
     * @param[in] port_identifier port identifier
     * */
    SwitchPortInfo(const std::string& port_identifier);

    /*!
     * @brief Default destructor.
     * */
    virtual ~SwitchPortInfo();

    /*!
     * @brief Gets Port present flag
     *
     * @return present flag.
     * */
    bool get_is_present() const {
        return m_is_present;
    }

    /*!
     * @brief Sets Port present flag
     *
     * @param is_present Port present plag.
     * */
    void set_is_present(const bool is_present) {
        m_is_present = is_present;
    }

    /*!
     * @brief Set public VLANs
     *
     * @param public_vlans Public VLANs
     * */
    void set_public_vlans(const PublicVlans& public_vlans) {
        m_public_vlans = public_vlans;
    }

    /*!
     * @brief Get public VLANs
     *
     * @return public VLANs
     * */
    PublicVlans get_public_vlans() const {
        return m_public_vlans;
    }

    /*!
     * @brief Gets switch port link state.
     *
     * @return Switch port link state.
     * */
    std::string get_link_state() const {
        return m_link_state.to_string();
    }

    /*!
     * @brief Gets switch port link state.
     *
     * @return Switch port link state.
     * */
    State get_link_state_enum() const {
        return m_link_state;
    }

    /*!
     * @brief Sets switch port link state.
     *
     * @param link_state Port link state.
     * */
    void set_link_state(const std::string& link_state) {
        m_link_state = State::from_string(link_state);
    }

    /*!
     * @brief Sets switch port link state using enum value.
     *
     * @param link_state Port link state.
     * */
    void set_link_state(State link_state) {
        m_link_state = link_state;
    }

    /*!
     * @brief Gets switch port operational state.
     *
     * @return Switch port operational state.
     * */
    std::string get_operational_state() const {
        return m_operational_state.to_string();
    }

    /*!
     * @brief Sets switch port operational state.
     *
     * @param operational_state Port operational state.
     * */
    void set_operational_state(const std::string& operational_state) {
        m_operational_state = State::from_string(operational_state);
    }

    /*!
     * @brief Sets switch port operational state.
     *
     * @param operational_state Port operational state.
     * */
    void set_operational_state(State operational_state) {
        m_operational_state = operational_state;
    }

    /*!
     * @brief Gets switch port link speed.
     *
     * @return Switch port link speed.
     * */
    std::uint32_t get_link_speed() const {
        return m_link_speed;
    }

    /*!
     * @brief Sets switch port link speed
     *
     * @param link_speed Port link speed.
     * */
    void set_link_speed(std::uint32_t link_speed) {
        m_link_speed = link_speed;
    }

    /*!
     * @brief Gets switch port max frame size.
     *
     * @return Switch port max frame size.
     * */
    std::uint32_t get_max_frame_size() const {
        return m_max_frame_size;
    }

    /*!
     * @brief Sets switch port max frame size
     *
     * @param max_frame_size Port max frame size.
     * */
    void set_max_frame_size(std::uint32_t max_frame_size) {
        m_max_frame_size = max_frame_size;
    }

    /*!
     * @brief Gets switch port identifier.
     *
     * @return Switch port identifier.
     * */
    std::string get_identifier() const {
        return m_identifier;
    }

    /*!
     * @brief Sets switch port identifier
     *
     * @param identifier Port identifier.
     * */
    void set_identifier(std::string identifier) {
        m_identifier = identifier;
    }

    /*!
     * @brief Gets switch port MAC address.
     *
     * @return Switch port MAC address.
     * */
    const std::string& get_mac_address() const {
        return m_mac_address;
    }

    /*!
     * @brief Sets switch port MAC address
     *
     * @param mac_address Port MAC address.
     * */
    void set_mac_address(const std::string& mac_address) {
        m_mac_address = mac_address;
    }

    /*!
     * @brief Checks if the link state is valid.
     *
     * @return true if the link state is valid, false otherwise.
     * */
    bool link_state_is_valid() const {
        return State::Unknown != get_link_state_enum();
    }

    /*!
     * @brief Get vlan list on the port.
     *
     * @return Vlan list.
     * */
    const VlanList& get_vlan_list() const {
        return m_vlan_list;
    }

    /*!
     * @brief Set vlan list on the port.
     *
     * @param[in] vlan_list Vlan list.
     * */
    void set_vlan_list(const VlanList& vlan_list) {
        m_vlan_list = vlan_list;
    }

    /*!
     * @brief Gets default vlan index.
     *
     * @return Vlan default index.
     * */
    VlanId get_default_vlan_id() const {
        return m_default_vlan_id;
    }

    /*!
     * @brief Set default vlan index.
     *
     * @param[in] vlan_id Vlan Id.
     * */
    void set_default_vlan_id(VlanId vlan_id) {
        m_default_vlan_id = vlan_id;
    }

    /*!
     * @brief Gets PFC enabled state.
     *
     * @return PFC enabled state.
     * */
    bool get_pfc_enabled() const {
        return m_pfc_enabled;
    }

    /*!
     * @brief Set PFC enabled state.
     *
     * @param[in] pfc_enabled PFC enabled state.
     * */
    void set_pfc_enabled(bool pfc_enabled) {
        m_pfc_enabled = pfc_enabled;
    }

    /*!
     * @brief Get port attribute type by string.
     *
     * @param[in] attr Port attribute type as string.
     *
     * @return Port attribtue.
     * */
    PortAttributeType get_port_attribute_type(const std::string& attr, const json::Json& value);

    /*!
     * Get PFC enabled priorities
     * @return A vector of priorities (uint32_t) if priorities are enabled, empty vector otherwise
     */
    const PfcEnabledPriorities& get_pfc_enabled_priorities() {
        return m_pfc_enabled_priorities;
    }

protected:
    std::string m_ipv4_subnet_mask{};

private:
    std::map<std::string, PortAttributeType> m_attribute_map {
        {"administrativeState", ADMINISTRATIVESTATE},
        {"linkSpeedMbps", LINKSPEEDMBPS},
        {"frameSize", FRAMESIZE},
        {"defaultVlan", DEFAULTVID},
        {"autoSense", AUTOSENSE},
        {"pfcEnabled", PFC_ENABLED},
        {"pfcEnabledPriorities", PFC_ENABLED_PRIORITIES},
        {"lldpEnabled", LLDP_ENABLED},
        {"dcbxState", DCBX_STATE}
    };

    bool m_is_present{false};

    VlanList m_vlan_list{};
    State m_link_state{State::Unknown};
    State m_operational_state{State::Unknown};
    std::uint32_t m_link_speed{0};
    std::uint32_t m_max_frame_size{0};
    std::string m_identifier{"Unknown"};
    std::string m_mac_address{};
    VlanId m_default_vlan_id{0};
    PfcEnabledPriorities m_pfc_enabled_priorities{};
    bool m_pfc_enabled{false};
    PublicVlans m_public_vlans{};
};

}
}
}
