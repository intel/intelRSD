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
 * @file fdb_info_message.hpp
 *
 * @brief FDB Info Netlink message
 * */

#pragma once

#include "netlink/neigh_message.hpp"

#include <vector>

namespace netlink_base {

/*!
 * @brief FDB Info Netlink Message
 */
class FdbInfoMessage : public NeighMessage {
public:
    /*!
     * @enum EntryType
     * @brief Entry type
     *
     * @var EntryType::UNKNOWN
     * Unknown entry type
     *
     * @var EntryType::STATIC
     * Static FDB entry
     *
     * @var EntryType::DYNAMIC
     * Dynamic FDB entry
     *
     * */
    enum class EntryType {
        UNKNOWN,
        STATIC,
        DYNAMIC
    };

    /*!
     * @brief FDB inforamation
     */
    class EntryInfo final {
    public:
        /*!
         * @brief Default constructor
         *
         * @param[in] mac MAC address
         * @param[in] vlan Vlan id associated with this mac
         * @param[in] type Entry type
         * */
        explicit EntryInfo(const std::string& mac, std::uint16_t vlan, EntryType type) :
            m_mac{mac}, m_vlan{vlan}, m_type{type} { }

        /*!
         * @brief Get mac address
         *
         * @return MAC address
         * */
        const std::string& get_mac() const { return m_mac; }

        /*!
         * @brief Get VLAN id
         *
         * @return Vlan id
         * */
        std::uint16_t get_vlan() const { return m_vlan; }

        /*!
         * @brief Get entry type
         *
         * @return Entry type
         * */
        EntryType get_type() const { return m_type; }

    private:
        std::string m_mac;
        std::uint16_t m_vlan;
        EntryType m_type;
    };

    /*!
     * @brief Default constructor.
     *
     * @param[in] ifname Interface name
     */
    FdbInfoMessage(const std::string& ifname);

    /*!
     * @brief Get neighbors info
     *
     * @return Neighbors info
     */
    const std::vector<EntryInfo>& get_entries() const { return m_entries; }

    /*!
     * @brief Default destructor.
     */
    virtual ~FdbInfoMessage();

protected:
    /*!
     * @brief Prepare neighbor message
     */
    void prepare_neigh_message(struct nl_msg*) override;

    /*!
     * @brief Message handler
     */
    void process_neigh_message(struct nl_msg*, const struct ndmsg*) override;

private:
    std::vector<EntryInfo> m_entries{};
};

}
