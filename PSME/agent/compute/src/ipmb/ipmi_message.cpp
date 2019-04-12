/*!
 * @brief IPMI message implementation.
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
 * @file ipmi_message.cpp
 */

#include <ipmb/ipmi_message.hpp>
#include <ipmb/utils.hpp>

#include <logger/logger_factory.hpp>



using namespace agent::compute::ipmb;
using namespace agent::compute::ipmb::utils;


void IpmiMessage::set_data(const uint8_t* data, const uint16_t len) {
    if (data && len > 0) {
        std::copy(data, data + len, m_data);
    }
}


std::vector<uint8_t> IpmiMessage::as_byte_array() {
    std::vector<uint8_t> buffer(get_len());

    buffer[IPMB_OFFSET_DEST_ADDR] = get_dest();
    if (m_is_request) {
        buffer[IPMB_OFFSET_NETFN_DLUN] = IPMI_MSG_SET_REQ_NETFN_DLUN(get_netfn(), get_dlun());
    }
    else {
        buffer[IPMB_OFFSET_NETFN_DLUN] = IPMI_MSG_SET_RSP_NETFN_DLUN(get_netfn(), get_dlun());
    }
    buffer[IPMB_OFFSET_HDR_CHCKSUM] = get_h_chcksum();
    buffer[IPMB_OFFSET_SRC_ADDR] = get_src();
    buffer[IPMB_OFFSET_SEQ_SLUN] = IPMI_MSG_SET_SEQ_SLUN(get_seq(), get_slun());
    buffer[IPMB_OFFSET_CMD] = get_cmd();

    auto data = get_data();

    if (uint8_t(CompletionCode::CC_OK) != data[0]) {
        buffer[IPMB_OFFSET_DATA] = data[0];

        buffer[IPMB_OFFSET_DATA_CHCKSUM] = utils::ipmi_checksum(&buffer[IPMB_OFFSET_SRC_ADDR],
                                                                uint16_t(IPMB_ERR_FRAM_PAYLOAD_SIZE));
        m_len = IPMB_FRAME_HDR_WITH_DATA_CHCKSUM_LEN + 1;
    }
    else {
        for (uint16_t i = 0; i < get_len() - IPMB_FRAME_HDR_WITH_DATA_CHCKSUM_LEN; i++) {
            buffer[IPMB_OFFSET_DATA + i] = data[i];
        }

        // To find data paylaod size, we have to substract SlaveAddres,
        // NetFunction/DestLUN, HeaderChecksum and DataChecksum bytes (4)
        // from full frame size.
        buffer[get_len() - 1] = utils::ipmi_checksum(&buffer[IPMB_OFFSET_SRC_ADDR], uint16_t(get_len() - 4));
    }
    buffer[IPMB_OFFSET_HDR_CHCKSUM] = utils::ipmi_checksum(buffer.data(), IPMI_MSG_HEADER_SIZE);

    return buffer;
}


const std::string IpmiMessage::print_msg() const {
    std::string msg_body_string{"IPMI Message - "};

    if (get_len() < IPMB_FRAME_HDR_LEN) {
        msg_body_string += "corrupt as message length(" +
                           std::to_string(static_cast<unsigned>(get_len())) + ") < " +
                           std::to_string(static_cast<unsigned>(IPMB_FRAME_HDR_LEN));
        return msg_body_string;
    }
    msg_body_string += "dest: " + std::to_string(unsigned(get_dest()));
    msg_body_string += ", dlun: " + std::to_string(unsigned(get_dlun()));
    msg_body_string += ", netfn: " + std::to_string(unsigned(get_netfn()));
    msg_body_string += ", hchck: " + std::to_string(unsigned(get_h_chcksum()));
    msg_body_string += ", src: " + std::to_string(unsigned(get_src()));
    msg_body_string += ", slun: " + std::to_string(unsigned(get_slun()));
    msg_body_string += ", seq: " + std::to_string(unsigned(get_seq()));
    msg_body_string += ", cmd: " + std::to_string(unsigned(get_cmd()));
    msg_body_string += ", dchck: " + std::to_string(unsigned(get_d_chcksum()));

    msg_body_string += ", data: ";

    auto data = get_data();

    for (uint32_t i = 0; i < uint32_t(get_len() - IPMB_FRAME_HDR_LEN); i++) {
        msg_body_string += " " + std::to_string(unsigned(data[i]));
    }

    msg_body_string += ", len: " + std::to_string(unsigned(get_len()));

    return msg_body_string;
}
