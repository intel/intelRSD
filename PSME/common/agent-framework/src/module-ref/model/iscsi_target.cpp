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
 * */

#include "agent-framework/module-ref/model/iscsi_target.hpp"
#include "agent-framework/module-ref/constants/common.hpp"
#include "agent-framework/module-ref/constants/storage.hpp"
#include "agent-framework/module-ref/utils/utils.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component IscsiTarget::component =
    enums::Component::IscsiTarget;
const enums::CollectionName IscsiTarget::collection_name =
    enums::CollectionName::iSCSITargets;

IscsiTarget::IscsiTarget(const std::string& parent_uuid) :
    Resource{parent_uuid, enums::Component::StorageServices} {}

IscsiTarget::~IscsiTarget() {}

Json::Value IscsiTarget::to_json() const {
    Json::Value result;
    result[literals::IscsiTarget::STATUS] = get_status().to_json();
    result[literals::IscsiTarget::INITIATOR_IQN] = get_initiator_iqn();
    result[literals::IscsiTarget::TARGET_ADDRESS] = get_target_address();
    result[literals::IscsiTarget::TARGET_PORT] = get_target_port();
    result[literals::IscsiTarget::TARGET_IQN] = get_target_iqn();
    result[literals::IscsiTarget::TARGET_LUNS] = get_target_lun().to_json();
    result[literals::IscsiTarget::OEM] = get_oem().to_json();
    return result;
}

IscsiTarget IscsiTarget::from_json(const Json::Value& json) {
    IscsiTarget target;

    target.set_status(attribute::Status::from_json(
        json[literals::IscsiTarget::STATUS]));
    target.set_initiator_iqn(json[literals::IscsiTarget::INITIATOR_IQN]);
    target.set_target_address(json[literals::IscsiTarget::TARGET_ADDRESS]);
    target.set_target_port(json[literals::IscsiTarget::TARGET_PORT]);
    target.set_target_iqn(json[literals::IscsiTarget::TARGET_IQN]);
    target.set_target_lun(TargetLuns::from_json(
        json[literals::IscsiTarget::TARGET_LUNS]));
    target.set_oem(attribute::Oem::from_json(json[literals::IscsiTarget::OEM]));
    target.set_resource_hash(compute_hash(json));

    return target;
}
