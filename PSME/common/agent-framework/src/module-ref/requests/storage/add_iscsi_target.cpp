/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 *
 * @file requests/compute/add_iscsi_target.cpp
 *
 * @brief Storage AddIscsiTarget request implementation
 * */

#include "agent-framework/module-ref/requests/storage/add_iscsi_target.hpp"
#include "agent-framework/module-ref/constants/storage.hpp"
#include "agent-framework/module-ref/constants/common.hpp"
#include <json/json.h>

using namespace agent_framework::model::requests;
using namespace agent_framework::model::literals;

AddIscsiTarget::AddIscsiTarget(const std::string& initiator_iqn,
                               const std::string& target_iqn,
                               agent_framework::model::attribute::Oem) :
    m_initiator_iqn(initiator_iqn), m_target_iqn(target_iqn), m_oem{} {}

Json::Value AddIscsiTarget::to_json() const {
    Json::Value value;
    value[IscsiTarget::INITIATOR_IQN] = m_initiator_iqn;
    value[IscsiTarget::TARGET_IQN] = m_target_iqn;
    value[IscsiTarget::TARGET_LUNS] = m_target_luns.to_json();
    value[Oem::OEM] = m_oem.to_json();
    return value;
}

AddIscsiTarget AddIscsiTarget::from_json(const Json::Value& json) {
    AddIscsiTarget request{
        json[IscsiTarget::INITIATOR_IQN].asString(),
        json[IscsiTarget::TARGET_IQN].asString(),
        agent_framework::model::attribute::Oem{}
    };
    request.set_target_luns(TargetLuns::from_json(json[IscsiTarget::TARGET_LUNS]));

    return request;
}
