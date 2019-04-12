/*!
 * @brief Set OOB NIC Boot Options command declaration.
 *
 * @copyright Copyright (c) 2016-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file set_oob_nic_boot_options.hpp
 */

#pragma once



#include "set_oob_boot_options.hpp"



namespace ipmi {
namespace command {
namespace sdv {

namespace request {

/*!
 * @brief Request message for IPMI Set OOB NIC Boot Options.
 */
class SetOobNicBootOptions : public SetOobBootOptions {
public:

    /*!
     * @brief Default constructor.
     */
    SetOobNicBootOptions();


    /*! Copy constructor */
    SetOobNicBootOptions(const SetOobNicBootOptions&) = default;


    /*! Assignment operator */
    SetOobNicBootOptions& operator=(const SetOobNicBootOptions&) = default;


    /*!
     * @brief Default destructor.
     */
    virtual ~SetOobNicBootOptions();


    virtual void pack(std::vector<uint8_t>& data) const override;


    void set_dhcp_enabled(bool enabled) { m_dhcp_enabled = enabled; }


    void set_dhcp_ip_version(IPVersion version) { m_dhcp_ip_version = version; }


    void set_address(const std::string& address) { m_ip_address = address; }


    void set_subnet(const std::string& subnet) { m_subnet_mask = subnet; }


    void set_gateway(const std::string& gateway) { m_gateway = gateway; }


    void set_primary_dns(const std::string& primary_dns) { m_primary_dns = primary_dns; }


    void set_secondary_dns(const std::string& secondary_dns) { m_secondary_dns = secondary_dns; }


    void set_name_offset(uint16_t offset) { m_name_offset = offset; }


    void set_name_length(uint16_t length) { m_name_length = length; }


    void set_vlan_enabled(bool enabled) { m_vlan_enabled = enabled; }


    void set_vlan(uint16_t id) { m_vlan = id; }


    void set_mac(const std::string& mac) { m_mac = mac; }

    virtual const char* get_command_name() const override {
        return "SetOobNicBootOptions";
    }

private:

    static constexpr std::uint8_t NIC_PARAMETER_INDEX = 0x00;

    bool m_dhcp_enabled{false};
    IPVersion m_dhcp_ip_version{IPVersion::AUTO};
    std::string m_ip_address{NULL_ADDRESS};
    std::string m_subnet_mask{NULL_ADDRESS};
    std::string m_gateway{NULL_ADDRESS};
    std::string m_primary_dns{NULL_ADDRESS};
    std::string m_secondary_dns{NULL_ADDRESS};
    uint16_t m_name_offset{0};
    uint16_t m_name_length{0};
    bool m_vlan_enabled{false};
    uint16_t m_vlan{0};
    std::string m_mac{NULL_MAC};

};

}

namespace response {

/*!
 * @brief Response message for IPMI SetOOBNIC Boot option.
 */
class SetOobNicBootOptions : public SetOobBootOptions {
public:

    /*!
     * @brief Default constructor.
     */
    SetOobNicBootOptions();


    /*! Copy constructor */
    SetOobNicBootOptions(const SetOobNicBootOptions&) = default;


    /*! Assignment operator */
    SetOobNicBootOptions& operator=(const SetOobNicBootOptions&) = default;


    /*!
     * @brief Default destructor.
     */
    virtual ~SetOobNicBootOptions();


    virtual void unpack(const std::vector<uint8_t>& data) override;

    virtual const char* get_command_name() const override {
        return "SetOobNicBootOptions";
    }

private:
    static constexpr size_t RESPONSE_SIZE = 1;
};

}

}
}
}
