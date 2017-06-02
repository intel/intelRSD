/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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

#include "agent-framework/module/requests/storage/add_iscsi_target.hpp"
#include "agent-framework/module/constants/storage.hpp"
#include "agent-framework/module/constants/common.hpp"
#include <json/json.h>

using namespace agent_framework::module::utils;
using namespace agent_framework::model::requests;
using namespace agent_framework::model::literals;

Json::Value AddIscsiTarget::to_json() const {
    Json::Value value;
    value[IscsiTarget::INITIATOR_IQN] = m_initiator_iqn;
    value[IscsiTarget::TARGET_IQN] = m_target_iqn;
    value[IscsiTarget::TARGET_LUNS] = m_target_luns.to_json();
    value[IscsiTarget::AUTHENTICATION_METHOD] = m_authentication_method;
    value[IscsiTarget::CHAP_USERNAME] = m_chap_username;
    value[IscsiTarget::CHAP_SECRET] = m_chap_secret;
    value[IscsiTarget::MUTUAL_CHAP_USERNAME] = m_mutual_chap_username;
    value[IscsiTarget::MUTUAL_CHAP_SECRET] = m_mutual_chap_secret;
    value[Oem::OEM] = m_oem.to_json();
    return value;
}

AddIscsiTarget AddIscsiTarget::from_json(const Json::Value& json) {
    AddIscsiTarget request{};
    request.set_initiator_iqn(json[IscsiTarget::INITIATOR_IQN]);
    request.set_target_iqn(json[IscsiTarget::TARGET_IQN].asString());
    request.set_target_luns(TargetLuns::from_json(json[IscsiTarget::TARGET_LUNS]));
    request.set_authentication_method(json[IscsiTarget::AUTHENTICATION_METHOD]);
    request.set_chap_username(json[IscsiTarget::CHAP_USERNAME]);
    request.set_chap_secret(json[IscsiTarget::CHAP_SECRET]);
    request.set_mutual_chap_username(json[IscsiTarget::MUTUAL_CHAP_USERNAME]);
    request.set_mutual_chap_secret(json[IscsiTarget::MUTUAL_CHAP_SECRET]);
    request.set_oem(attribute::Oem::from_json(json[IscsiTarget::OEM]));

    return request;
}
