/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 *
 * @file ipmi_commands.cpp
 * @brief IPMI commands handlers for RMM.
 * */

#include <ipmb/command/get_device_id.hpp>
#include <ipmb/ipmi_message.hpp>
#include <ipmb/utils.hpp>

#include <logger/logger_factory.hpp>

using namespace agent::chassis::ipmb;
using namespace agent::chassis::ipmb::command;

GetDeviceID::~GetDeviceID() {}

void GetDeviceID::Response::add_data(IpmiMessage& msg) {
    auto data = msg.get_data();

    data[OFFSET_CC] = uint8_t(m_cc);
    data[OFFSET_DEV_ID] = m_dev_id;
    data[OFFSET_DEV_REV] = m_dev_rev;
    data[OFFSET_REV_MAJOR] = m_rev_major;
    data[OFFSET_REV_MINOR] = m_rev_minor;
    data[OFFSET_IPMI_VER] = m_ipmi_ver;
    data[OFFSET_DEV_SUPP] = m_dev_supp;
    data[OFFSET_MAN_ID_0] = m_man_id.at(0);
    data[OFFSET_MAN_ID_1] = m_man_id.at(1);
    data[OFFSET_MAN_ID_2] = m_man_id.at(2);

    msg.add_len(get_len());
}

void GetDeviceID::unpack(IpmiMessage& msg){
    log_debug(LOGUSR, "Unpacking Get Device ID message.");
    msg.set_to_request();
    m_response.set_cc(CompletionCode::CC_OK);
}

void GetDeviceID::pack(IpmiMessage& msg){
    std::vector<uint8_t> man_id{};
    std::vector<uint8_t> aux_fw_rev{};

    log_debug(LOGUSR, "Packing Get Device ID message.");

    m_response.set_dev_id(IPMI_DEVICE_ID);
    m_response.set_dev_rev(IPMI_DEVICE_REV);
    m_response.set_rev_major(IPMI_DEVICE_REV_MAJOR());
    m_response.set_rev_minor(IPMI_DEVICE_REV_MINOR());
    m_response.set_dev_supp(IPMI_CHASSIS_DEVICE);
    m_response.set_ipmi_ver(IPMI_DEVICE_IPMI_VER);

    man_id.push_back(IPMI_IANA_INTEL_0());
    man_id.push_back(IPMI_IANA_INTEL_1());
    man_id.push_back(IPMI_IANA_INTEL_2());
    m_response.set_man_id(man_id);

    const auto fw_rev = IPMI_AUX_FW_REV();
    aux_fw_rev.push_back(uint8_t(fw_rev));
    aux_fw_rev.push_back(uint8_t(fw_rev >> 8));
    aux_fw_rev.push_back(uint8_t(fw_rev >> 16));
    aux_fw_rev.push_back(uint8_t(fw_rev >> 24));
    m_response.set_aux_fw_rev(aux_fw_rev);

    msg.set_cc(CompletionCode::CC_OK);
    msg.set_to_response();

    m_response.set_len(CMD_RESPONSE_DATA_LENGHT);

    m_response.add_data(msg);
}


