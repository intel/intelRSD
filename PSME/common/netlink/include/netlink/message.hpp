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
 * @file message.hpp
 *
 * @brief Netlink message
 * */

#pragma once

#include "agent-framework/module-ref/enum/enum_builder.hpp"
#include <netlink/genl/genl.h>

#include <memory>
#include <map>
#include <string>

namespace netlink_base {

ENUM(State, uint32_t, Up, Down, Unknown);

/*!
 * @brief Netlink socket.
 */
class Message {
public:
    struct Deleter {
        void operator()(struct nl_msg* msg) const {
            nlmsg_free(msg);
        }
    };

    using IfName = std::string;
    using Pointer = std::unique_ptr<struct nl_msg, Deleter>;
    using Vlan = uint16_t;
    using Type = uint16_t;
    using Flags = uint32_t;

    /*! Netlink header class */
    class NetlinkHeader {
    private:
        struct nlmsghdr* p_hdr;
    public:
        /*!
         * @brief Default constructor.
         */
        NetlinkHeader() = default;

        /*!
         * @brief Default constructor.
         *
         * @param[in] p Pointer to netlink message.
         */
        NetlinkHeader(struct nlmsghdr* p) : p_hdr(p) { }

        /*!
         * @brief Get netlink data.
         *
         * @return Netlink data.
         */
        void* get_data() const { return nlmsg_data(p_hdr); }

        /*!
         * @brief Get netlink message length.
         *
         * @return Netlink message length.
         */
         __u32 get_len() const { return p_hdr->nlmsg_len; }

        /*!
         * @brief Get netlink message type.
         *
         * @return Netlink message type.
         */
         __u16 get_type() const { return p_hdr->nlmsg_type; }
    };

    /*!
     * @brief Default constructor.
     */
    Message();

    /*!
     * @brief Default constructor.
     *
     * @param[in] ifname Interface name.
     */
    Message(const IfName& ifname);

    /*!
     * @brief Default destructor.
     */
    virtual ~Message();

    /*!
     * @brief Copy constructor.
     *
     * @param[in] orig object to be copied.
     */
    Message(const Message& orig) = delete;

    /*!
     * @brief Copy operator.
     *
     * @param[in] s object to be copied
     */
    Message& operator= (const Message& s) = delete;

    /*!
     * @brief Set type of message.
     *
     * @param[in] type Netlink message type to be set.
     */
    void set_type(Type type) { m_type = type; }

    /*!
     * @brief Set flags of message
     *
     * @param[in] flags Netlink flags to be set.
     */
    void set_flags(Flags flags) { m_flags = flags; }

    /*!
     * @brief Set port state.
     *
     * @param[in] state Port state.
     */
    void set_link_state(State state) {
        m_link_state = state;
    }

    /*!
     * @brief Get port state.
     *
     * @return state Port state.
     */
    State get_link_state() const { return m_link_state; }

    /*!
     * @brief Set operational port state.
     *
     * @param[in] state Operational port state.
     */
    void set_operational_state(State state) {
        m_operational_state = state;
    }

    /*!
     * @brief Get operational port state.
     *
     * @return state Operational port state.
     */
    State get_operational_state() const {
        return m_operational_state;
    }

    /*!
     * @brief Prepare rtnl message to be sent.
     *
     * @return Netlink message ready to be sent.
     */
    Pointer prepare_rtnl_msg();

    /*!
     * @brief Prepare netlink message to be sent.
     *
     * @return Netlink message ready to be sent.
     */
    virtual Pointer prepare_netlink_msg() const {
        return nullptr;
    }

    /*!
     * @brief Parse recv netlink header.
     *
     * @param[in] nlhdr Netlink header.
     */
    virtual void parse_netlink_hdr(NetlinkHeader& nlhdr) { (void)nlhdr; }

    /*!
     * @brief Check if the message is the last in the series of messages.
     *
     * @return true if the message is last, false otherwise.
     */
    bool get_last() { return m_last; }

    /*!
     * @brief Set the message as the last in the series of messages.
     */
    void set_last() { m_last = true; }

    /*!
     * @brief Get interface index.
     *
     * @return interface index.
     */
    int get_ifindex() { return m_ifindex; }

    /*!
     * @brief set MAC address.
     *
     * @param[in] str MAC address to be set.
     */
    void set_address(const std::string& str) { m_address = str; }

    /*!
     * @brief get MAC address.
     *
     * @return MAC address.
     */
    std::string& get_address() { return m_address; }

    /*!
     * @brief Read neighbour from the netlink message.
     *
     * @return returns NL_OK.
     */
    static int read_neighbour(struct nl_msg *nlmsg, void *arg);

    /*!
     * @brief parse netlink attributes.
     *
     * @param[in] tb the resulting table with attributes.
     * @param[in] max maximum number of elements in tb.
     * @param[in] rta the attributes as received from the network.
     * @param[in] len the number of attributes in rta.
     */
    static void parse_attr(struct rtattr *tb[], unsigned int max,
                           struct rtattr *rta, int len);

protected:
    /*! Netlink message type */
    Type m_type{RTM_GETLINK};

    /*! Netlink message flags */
    Flags m_flags{};

    /*! Netlink interface name */
    IfName m_ifname{};

    /*! IFLA_SWPORT_ATTRS and AFLA_ATTR_* definitions are
     * in if_link.h linux header file of NOS linux image.
     * Temporary define them here.
     */
    static constexpr uint16_t IFLA_SWPORT_ATTRS = 40;

    /*! @brief ifi_change (ifinfomsg struct) is reserved for
     *         future use and always should be set to 0xFFFFFFFF
     */
    static constexpr uint32_t IFI_CHANGE_RESERVED = 0xffffffff;

private:
    /*!
     * @brief Prepare getneigh rtnl message to be sent.
     *
     * @param[out] msg already allocated message.
     *
     * @return true if message is ready to be sent, false otherwise.
     */
    bool prepare_getneigh_msg(Pointer& msg);

    State m_link_state{State::Unknown};
    static constexpr uint16_t ARPHRD_ETHER = 1;
    State m_operational_state{State::Unknown};
    bool m_last{false};
    int m_ifindex{};
    std::string m_address{};
};

}

