/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * */

#include "agent-framework/module/constants/common.hpp"
#include "agent-framework/module/model/attributes/ip_transport_detail.hpp"

using namespace agent_framework::model::attribute;
using namespace agent_framework::model;

IpTransportDetail::~IpTransportDetail() { }

json::Json IpTransportDetail::to_json() const {
    json::Json result = json::Json();
    result[literals::IpTransportDetail::IPV4_ADDRESS] = get_ipv4_address().to_json();
    result[literals::IpTransportDetail::IPV6_ADDRESS] = get_ipv6_address().to_json();
    result[literals::IpTransportDetail::INTERFACE] = get_interface();
    result[literals::IpTransportDetail::PORT] = get_port();
    result[literals::IpTransportDetail::PROTOCOL] = get_transport_protocol();
    return result;
}

IpTransportDetail IpTransportDetail::from_json(const json::Json& json) {
    attribute::IpTransportDetail td{};
    td.set_ipv4_address(Ipv4Address::from_json(json[literals::IpTransportDetail::IPV4_ADDRESS]));
    td.set_ipv6_address(Ipv6Address::from_json(json[literals::IpTransportDetail::IPV6_ADDRESS]));
    td.set_interface(json[literals::IpTransportDetail::INTERFACE]);
    td.set_port(json[literals::IpTransportDetail::PORT]);
    td.set_transport_protocol(json[literals::IpTransportDetail::PROTOCOL]);
    return td;
}
