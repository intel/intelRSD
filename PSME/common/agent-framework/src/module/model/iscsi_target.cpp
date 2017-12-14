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
 * */

#include "agent-framework/module/model/iscsi_target.hpp"
#include "agent-framework/module/constants/storage.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component IscsiTarget::component =
    enums::Component::IscsiTarget;
const enums::CollectionName IscsiTarget::collection_name =
    enums::CollectionName::iSCSITargets;

IscsiTarget::IscsiTarget(const std::string& parent_uuid, enums::Component parent_type) :
    Resource{parent_uuid, parent_type} {}

IscsiTarget::~IscsiTarget() {}

json::Json IscsiTarget::to_json() const {
    json::Json result;
    result[literals::IscsiTarget::STATUS] = get_status().to_json();
    result[literals::IscsiTarget::INITIATOR_IQN] = get_initiator_iqn();
    result[literals::IscsiTarget::TARGET_ADDRESS] = get_target_address();
    result[literals::IscsiTarget::TARGET_PORT] = get_target_port();
    result[literals::IscsiTarget::TARGET_IQN] = get_target_iqn();
    result[literals::IscsiTarget::TARGET_LUNS] = get_target_lun().to_json();
    result[literals::IscsiTarget::AUTHENTICATION_METHOD] = get_authentication_method();
    result[literals::IscsiTarget::CHAP_USERNAME] = get_chap_username();
    result[literals::IscsiTarget::CHAP_SECRET] = get_chap_secret();
    result[literals::IscsiTarget::MUTUAL_CHAP_USERNAME] = get_mutual_chap_username();
    result[literals::IscsiTarget::MUTUAL_CHAP_SECRET] = get_mutual_chap_secret();
    result[literals::IscsiTarget::OEM] = get_oem().to_json();
    return result;
}

IscsiTarget IscsiTarget::from_json(const json::Json& json) {
    IscsiTarget target;

    target.set_status(attribute::Status::from_json(
        json[literals::IscsiTarget::STATUS]));
    target.set_initiator_iqn(json[literals::IscsiTarget::INITIATOR_IQN]);
    target.set_target_address(json[literals::IscsiTarget::TARGET_ADDRESS]);
    target.set_target_port(json[literals::IscsiTarget::TARGET_PORT]);
    target.set_target_iqn(json[literals::IscsiTarget::TARGET_IQN]);
    target.set_target_lun(TargetLuns::from_json(
        json[literals::IscsiTarget::TARGET_LUNS]));
    target.set_authentication_method(json[literals::IscsiTarget::AUTHENTICATION_METHOD]);
    target.set_chap_username(json[literals::IscsiTarget::CHAP_USERNAME]);
    target.set_chap_secret(json[literals::IscsiTarget::CHAP_SECRET]);
    target.set_mutual_chap_username(json[literals::IscsiTarget::MUTUAL_CHAP_USERNAME]);
    target.set_mutual_chap_secret(json[literals::IscsiTarget::MUTUAL_CHAP_SECRET]);
    target.set_oem(attribute::Oem::from_json(json[literals::IscsiTarget::OEM]));
    target.set_resource_hash(json);

    return target;
}
