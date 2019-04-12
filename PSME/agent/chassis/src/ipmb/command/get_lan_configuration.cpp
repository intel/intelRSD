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

#include <ipmb/command/get_lan_configuration.hpp>
#include <ipmb/network_utils.hpp>
#include <ipmb/ipmi_message.hpp>
#include <ipmb/utils.hpp>
#include <ipmb/command/get-lan-selector/selectors.hpp>

#include <logger/logger_factory.hpp>

using namespace agent::chassis::ipmb::command;

GetLanConfiguration::GetLanConfiguration() :
    m_selectors {
        {Selector::IP, std::make_shared<IpSelector>()},
        {Selector::MASK, std::make_shared<MaskSelector>()},
        {Selector::SOURCE, std::make_shared<SourceSelector>()}
    } {
    m_current = m_invalid_selector;
}
GetLanConfiguration::~GetLanConfiguration() {}

void GetLanConfiguration::unpack(IpmiMessage& msg) {
    log_debug("ipmb", "Unpacking Get Lan Configuration message.");
    msg.set_to_request();
    process_selector(msg);
    m_current->unpack(msg);
}

void GetLanConfiguration::pack(IpmiMessage& msg) {
    m_current->pack(msg);
}

void GetLanConfiguration::process_selector(const IpmiMessage& msg) {
    auto data = msg.get_data();
    auto value = data[OFFSET_SELECTOR];

    if (m_selectors.find(value) != m_selectors.cend()) {
        m_current = m_selectors[value];
    } else {
        m_current = m_invalid_selector;
    }
}

