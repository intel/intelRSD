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
 * */

#include "ipmb/command/get-lan-selector/ip_selector.hpp"
#include "ipmb/ipmi_message.hpp"
#include "ipmb/utils.hpp"
#include "ipmb/network_utils.hpp"

#include "logger/logger_factory.hpp"
#include "configuration/configuration.hpp"

#include <algorithm>

using namespace agent::chassis::ipmb::command;



IpSelector::~IpSelector() {}

void IpSelector::Response::add_data(IpmiMessage& msg) {
    auto data = msg.get_data();

    data[OFFSET_CC] = uint8_t(m_cc);
    data[OFFSET_REVISTION] = m_param_revision;

    std::copy(m_ip.cbegin(), m_ip.cend(), data + OFFSET_ADDR);

    msg.add_len(get_len());
}

void IpSelector::unpack(IpmiMessage& msg) {
    msg.set_to_request();
}

void IpSelector::pack(IpmiMessage& msg){
    m_response.set_cc(CompletionCode::CC_OK);

    try {
        auto ip = network_utils::get_ip(network_utils::get_network_interface());

        log_debug(LOGUSR, "Read interface address : " << ip);
        auto bytes = network_utils::addr_to_bytes(ip);

        Response::IpBytes ip_addr{{}};
        std::copy(bytes.cbegin(), bytes.cend(), ip_addr.begin());
        m_response.set_ip_data(std::move(ip_addr));
    }
    catch (const std::runtime_error& e) {
        log_debug(LOGUSR, "Read interface information error : " << e.what());
        m_response.set_cc(CompletionCode::CC_CANNOT_RETURN_REQ_DATA);
    }
    catch (const std::invalid_argument& e) {
        log_debug(LOGUSR, "Read incorrect IP address : " << e.what());
        m_response.set_cc(CompletionCode::CC_CANNOT_RETURN_REQ_DATA);
    }

    m_response.set_len(CMD_RESPONSE_DATA_LENGHT);
    m_response.add_data(msg);
    msg.set_to_response();
}


