/*!
 * @brief Definition of Network QoS Priority to Priority Group Mapping.
 *        Used as DCBX Shared Configuration for ETS (Enhanced Transmission Selection).
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
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
 * @file qos_priority_group_mapping.cpp
 */

#include "agent-framework/module/model/attributes/qos_priority_group_mapping.hpp"
#include "agent-framework/module/constants/network.hpp"

using namespace agent_framework::model::attribute;
using namespace agent_framework::model;

QosPriorityGroupMapping::QosPriorityGroupMapping() {}
QosPriorityGroupMapping::~QosPriorityGroupMapping() {}

json::Json QosPriorityGroupMapping::to_json() const {
    json::Json result = json::Json();
    result[literals::NetworkQosAttribute::PRIORITY_GROUP] = QosPriorityGroupMapping::get_priority_group();
    result[literals::NetworkQosAttribute::PRIORITY] = QosPriorityGroupMapping::get_priority();
    return result;
}

QosPriorityGroupMapping QosPriorityGroupMapping::from_json(const json::Json& json) {
    QosPriorityGroupMapping priority_mapping{};
    priority_mapping.set_priority_group(json[literals::NetworkQosAttribute::PRIORITY_GROUP]);
    priority_mapping.set_priority(json[literals::NetworkQosAttribute::PRIORITY]);
    return priority_mapping;
}