/*!
 * @brief SPDK Endpoint builder class implementation.
 *
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation.
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
 * @file spdk_endpoint_builder.cpp
 */

#include "agent-framework/discovery/builders/identifier_builder.hpp"
#include "discovery/builders/spdk_endpoint_builder.hpp"

using namespace agent_framework;
using namespace agent_framework::discovery;
using namespace agent::spdk::discovery;

namespace {

static constexpr const char ENDPOINT_NAME[] = "SPDK NVMf endpoint";
static constexpr const char ENDPOINT_DESCRIPTION[] = "SPDK NVMf endpoint";

}


agent_framework::model::Endpoint SpdkEndpointBuilder::build_default(const Uuid& parent_uuid) {
    auto endpoint = EndpointBuilder::build_default(parent_uuid);
    endpoint.set_name(::ENDPOINT_NAME);
    endpoint.set_description(::ENDPOINT_DESCRIPTION);
    SpdkEndpointBuilder::nvmeof_protocol(endpoint);
    return endpoint;
}

void SpdkEndpointBuilder::update_target_with_spdk(agent_framework::model::Endpoint& endpoint,
                                                  const ::spdk::model::NvmfSubsystem& nvmf_subsystem) {
    IdentifierBuilder::set_nqn(endpoint, nvmf_subsystem.get_nqn());
}

void SpdkEndpointBuilder::add_ip_transport_details(agent_framework::model::Endpoint& endpoint,
                                                   const ::spdk::model::ListenAddress& listen_address,
                                                   const std::vector<Uuid>& system_interfaces) {

    agent_framework::model::attribute::IpTransportDetail transport{};
    transport.set_transport_protocol(agent_framework::model::enums::TransportProtocol::RoCEv2);

    if (listen_address.get_trsvcid()) {
        transport.set_port(std::stoi(listen_address.get_trsvcid().value()));
    }

    agent_framework::model::attribute::Ipv4Address address{};
    address.set_address(listen_address.get_traddr());
    transport.set_ipv4_address(address);

    if (!system_interfaces.empty()) {
        transport.set_interface(system_interfaces.front());
    }
    else {
        log_warning("storage-agent", "Cannot find interface with IP("
            << address.get_address() << ") in the system interfaces for endpoint: "
            << endpoint.get_uuid());

        endpoint.set_status(agent_framework::model::attribute::Status{
            agent_framework::model::enums::State::UnavailableOffline,
            agent_framework::model::enums::Health::Critical
        });
    }

    endpoint.add_ip_transport_detail(std::move(transport));
}