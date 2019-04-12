/*!
 * @brief Definition of Network QoS Bandwidth for Priority Group Allocation class.
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
 * @file qos_bandwidth_allocation.cpp
 */

#include "agent-framework/module/model/attributes/qos_bandwidth_allocation.hpp"
#include "agent-framework/module/constants/network.hpp"

using namespace agent_framework::model::attribute;
using namespace agent_framework::model;

QosBandwidthAllocation::QosBandwidthAllocation() {}
QosBandwidthAllocation::~QosBandwidthAllocation() {}

json::Json QosBandwidthAllocation::to_json() const {
    json::Json result = json::Json();
    result[literals::NetworkQosAttribute::PRIORITY_GROUP] = QosBandwidthAllocation::get_priority_group();
    result[literals::NetworkQosAttribute::BANDWIDTH_PERCENT] = QosBandwidthAllocation::get_bandwidth_percent();
    return result;
}

QosBandwidthAllocation QosBandwidthAllocation::from_json(const json::Json& json) {
    QosBandwidthAllocation bandwidth_allocation{};
    bandwidth_allocation.set_priority_group(json[literals::NetworkQosAttribute::PRIORITY_GROUP]);
    bandwidth_allocation.set_bandwidth_percent(json[literals::NetworkQosAttribute::BANDWIDTH_PERCENT]);
    return bandwidth_allocation;
}