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
 */

#include "tgt/config/tgt_target_config.hpp"
#include "tgt/config/tgt_target_lun_config.hpp"

#include <sstream>



using namespace tgt::config;

constexpr const char* TGT_CONFIG_PROPERTY_INITIATOR_NAME = "initiator-name";
constexpr const char* TAB = "\t";


const std::string TgtTargetConfig::to_string() const {
    const auto& target_iqn = m_target.get_target_iqn();
    if (target_iqn.empty()) {
        throw std::invalid_argument("Target iQN is empty!");
    }

    const auto& target_luns = m_target.get_luns();
    if (target_luns.empty()) {
        throw std::invalid_argument("No LUNs for target!");
    }

    std::ostringstream content{};
    content << get_target_begin_tag() << std::endl;
    for (const auto& lun : target_luns) {
        TgtTargetLunConfig lun_configuration(*lun);
        content << lun_configuration.to_string();
    }

    if (!m_target.get_target_initiator().empty()) {
        content << TAB << get_target_initiator_address() << std::endl;
    }
    content << get_target_end_tag() << std::endl;
    return content.str();
}


const std::string TgtTargetConfig::get_target_begin_tag() const {
    std::ostringstream content{};
    content << "<target " << m_target.get_target_iqn() << ">";
    return content.str();
}


const std::string TgtTargetConfig::get_target_end_tag() const {
    return std::string("</target>");
}


const std::string TgtTargetConfig::get_target_initiator_address() const {
    return TGT_CONFIG_PROPERTY_INITIATOR_NAME + std::string(" ") + m_target.get_target_initiator();
}

