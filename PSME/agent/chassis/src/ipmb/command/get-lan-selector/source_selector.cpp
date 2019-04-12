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

#include <ipmb/command/get-lan-selector/source_selector.hpp>
#include <ipmb/ipmi_message.hpp>
#include <ipmb/utils.hpp>
#include <ipmb/network_utils.hpp>

#include <logger/logger_factory.hpp>
#include "configuration/configuration.hpp"

using namespace agent::chassis::ipmb::command;

SourceSelector::~SourceSelector() {}

void SourceSelector::Response::add_data(IpmiMessage& msg) {
    auto data = msg.get_data();

    data[OFFSET_CC] = uint8_t(m_cc);
    data[OFFSET_REVISTION] = m_param_revision;
    data[OFFSET_SOURCE] = m_source;

    msg.add_len(get_len());
}

void SourceSelector::unpack(IpmiMessage& msg) {
    msg.set_to_request();
}

void SourceSelector::pack(IpmiMessage& msg){
    m_response.set_cc(CompletionCode::CC_OK);

    try {
        auto source = network_utils::get_source(network_utils::get_network_interface());

        m_response.set_source(static_cast<std::uint8_t>(source));
    }  catch (const std::runtime_error& e) {
        m_response.set_cc(CompletionCode::CC_CANNOT_RETURN_REQ_DATA);
        log_debug(LOGUSR, "Cannot read interface source!" << e.what());
    }

    m_response.set_len(CMD_RESPONSE_DATA_LENGHT);
    m_response.add_data(msg);
    msg.set_to_response();
}


