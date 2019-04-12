/*!
 * @brief Definition of Network QoS Application Protocol class.
 *        Used as DCBX Shared Configuration.
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
 * @file qos_application_protocol.cpp
 */

#include "agent-framework/module/model/attributes/qos_application_protocol.hpp"
#include "agent-framework/module/constants/network.hpp"

using namespace agent_framework::model::attribute;
using namespace agent_framework::model;

QosApplicationProtocol::QosApplicationProtocol() {}
QosApplicationProtocol::~QosApplicationProtocol() {}

json::Json QosApplicationProtocol::to_json() const {
    json::Json result = json::Json();
    result[literals::NetworkQosAttribute::PROTOCOL] = QosApplicationProtocol::get_protocol();
    result[literals::NetworkQosAttribute::PORT] = QosApplicationProtocol::get_port();
    result[literals::NetworkQosAttribute::PRIORITY] = QosApplicationProtocol::get_priority();
    return result;
}

QosApplicationProtocol QosApplicationProtocol::from_json(const json::Json& json) {
    QosApplicationProtocol application_protocol{};
    application_protocol.set_protocol(json[literals::NetworkQosAttribute::PROTOCOL]);
    application_protocol.set_port(json[literals::NetworkQosAttribute::PORT]);
    application_protocol.set_priority(json[literals::NetworkQosAttribute::PRIORITY]);
    return application_protocol;
}