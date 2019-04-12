/*!
 * @brief IPMI message interface.
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
 * @file ipmi_message.hpp
 */

#pragma once



#include "utils.hpp"

#include <cstdint>
#include <vector>

/*! Agent namspace */
namespace agent {
/*! Compute namspace */
namespace compute {
/*! IPMB namspace */
namespace ipmb {

using uint8_t = std::uint8_t;
using uint16_t = std::uint16_t;
using uint32_t = std::uint32_t;

/*! IPMI message */
class IpmiMessage {
public:
    /*! Maximum IPMI message length */
    static constexpr uint16_t IPMI_MAX_MSG_LENGTH = 272;
    /*! IPMB message header length */
    static constexpr uint8_t IPMI_HEADER_LENGTH = 6;

private:
    uint8_t m_dest{};
    uint8_t m_netfn{};
    uint8_t m_dlun{};
    uint8_t m_h_chcksum{};
    uint8_t m_src{};
    uint8_t m_slun{};
    uint8_t m_seq{};
    uint8_t m_cmd{};
    uint8_t m_cc{};
    uint8_t m_data[IPMI_MAX_MSG_LENGTH - IPMI_HEADER_LENGTH];
    uint8_t m_d_chcksum{};

    uint16_t m_len{};
    bool m_is_request{true};

public:
    /*! Default constructor */
    IpmiMessage() {}


    /*! Gets destination address
     *  @return Destination address. */
    uint8_t get_dest() const {
        return m_dest;
    }


    /*! Sets destination address
     *  @param dest destination address
     * */
    void set_dest(const uint8_t dest) {
        m_dest = dest;
    }


    /*! Gets Net function
     *  @return Net function */
    uint8_t get_netfn() const {
        return m_netfn;
    }


    /*! Sets Net function
     *  @param netfn Net function */
    void set_netfn(const uint8_t netfn) {
        m_netfn = netfn;
    }


    /*! Gets Destination LUN
     *  @return Destination LUN */
    uint8_t get_dlun() const {
        return m_dlun;
    }


    /*! Sets Destination LUN
     *  @param dlun Destination LUN */
    void set_dlun(const uint8_t dlun) {
        m_dlun = dlun;
    }


    /*! Gets header checksum
     *  @return header checksum */
    uint8_t get_h_chcksum() const {
        return m_h_chcksum;
    }


    /*! Sets header checksum
     *  @param h_chcksum header checksum */
    void set_h_chcksum(const uint8_t h_chcksum) {
        m_h_chcksum = h_chcksum;
    }


    /*! Gets Source address
     *  @return Source address */
    uint8_t get_src() const {
        return m_src;
    }


    /*! Sets Source address
     *  @param src Source address */
    void set_src(const uint8_t src) {
        m_src = src;
    }


    /*! Gets Source LUN
     *  @return Source LUN */
    uint8_t get_slun() const {
        return m_slun;
    }


    /*! Sets Source LUN
     *  @param slun Source LUN */
    void set_slun(const uint8_t slun) {
        m_slun = slun;
    }


    /*! Gets Source address
     *  @return Source address */
    uint8_t get_seq() const {
        return m_seq;
    }


    /*! Sets Sequence number
     *  @param seq Sequnce number */
    void set_seq(const uint8_t seq) {
        m_seq = seq;
    }


    /*! Gets Command
     *  @return Command */
    uint8_t get_cmd() const {
        return m_cmd;
    }


    /*! Sets Command
     *  @param cmd Command */
    void set_cmd(const uint8_t cmd) {
        m_cmd = cmd;
    }


    /*! Gets Completion Code
     *  @return Completion Code */
    uint8_t get_cc() const {
        return m_cc;
    }


    /*! Sets Completion Code
     *  @param cc Completion Code */
    void set_cc(const CompletionCode cc) {
        m_cc = uint8_t(cc);
    }


    /*! Gets data checksum
     *  @return data chceksum */
    uint8_t get_d_chcksum() const {
        return m_d_chcksum;
    }


    /*! Sets data chceksum
     *  @param d_chcksum data chcksum */
    void set_d_chcksum(const uint8_t d_chcksum) {
        m_d_chcksum = d_chcksum;
    }


    /*! Gets message length
     * @return Message length */
    uint16_t get_len() const {
        return m_len;
    }


    /*! Add message length
     * @param len Message length */
    void add_len(const uint16_t len) {
        if ((IPMI_MAX_MSG_LENGTH - m_len) < len) {
            m_len = IPMI_MAX_MSG_LENGTH;
        }
        else {
            m_len = uint16_t(m_len + len);
        }
    }


    /*! Marks message as request. */
    void set_to_request() {
        m_is_request = true;
    }


    /*! Marks message as response. */
    void set_to_response() {
        m_is_request = false;
    }


    /*! Gets is_request flag
     * @return Returns is_request flag */
    bool is_request() const {
        return m_is_request;
    }


    /*! Gets data
     * @return data pointer */
    uint8_t* get_data() {
        return m_data;
    }


    /*! Gets const data pointer
     * @return const data pointer */
    const uint8_t* get_data() const {
        return m_data;
    }


    /*! Sets data
     * @param data data to be set
     * @param len data length */
    void set_data(const uint8_t* data, const uint16_t len);


    /*! Print IPMI Message
     * @return formated string */
    const std::string print_msg() const;


    /*! Converts IPMI Message to buffer
     * @return buffer filled with IPMI message */
    std::vector<uint8_t> as_byte_array();
};

}
}
}

