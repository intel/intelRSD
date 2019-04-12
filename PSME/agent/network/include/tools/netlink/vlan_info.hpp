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
 * @file vlan_info.hpp
 *
 * @brief Vlan information
 * */

#pragma once

#include <cstdint>

namespace agent {
namespace network {
namespace tools {
namespace netlink {

/*! Vlan information class */
class VlanInfo final {
public:
    /*! Default constructor */
    VlanInfo() = default;

    /*!
     * @brief Default constructor
     *
     * @param[in] vlan Vlan id
     * @param[in] tagged Vlan tag
     * @param[in] pvid Default VLAN
     * */
    VlanInfo(uint32_t vlan, bool tagged, bool pvid) :
            m_vlan_id{vlan}, m_tagged{tagged}, m_pvid{pvid} { }

    /*!
     * @brief Set vlan id
     *
     * @param[in] vlan Vlan id
     * */
    void set_vlan_id(uint32_t vlan) { m_vlan_id = vlan; }

    /*!
     * @brief Get vlan id
     *
     * @return Vlan index
     * */
    uint32_t get_vlan_id() const { return m_vlan_id; }

    /*!
     * @brief Set vlan tag
     *
     * @param[in] tag Vlan tag (true/false)
     * */
    void set_tagged(bool tag) { m_tagged = tag; }

    /*!
     * @brief Get vlan tag
     *
     * @return Vlan tag (true/false)
     * */
    bool get_tagged() const { return m_tagged; }

    /*!
     * @brief Set vlan PVID
     *
     * @param[in] pvid Default VLAN
     * */
    void set_pvid(bool pvid) { m_pvid = pvid; }

    /*!
     * @brief Get vlan PVID
     *
     * @return Vlan PVID
     * */
    bool get_pvid() const { return m_pvid; }

private:
    uint32_t m_vlan_id;
    bool m_tagged;
    bool m_pvid;
};

}
}
}
}
