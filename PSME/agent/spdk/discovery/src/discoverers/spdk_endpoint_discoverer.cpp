/*!
 * @brief Endpoint discoverer implementation.
 *
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
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
 * @file spdk_endpoint_discoverer.cpp
 */

#include "agent-framework/module/storage_components.hpp"
#include "agent-framework/discovery/builders/identifier_builder.hpp"
#include "agent-framework/database/database_entities.hpp"
#include "agent-framework/database/database_keys.hpp"
#include "discovery/discoverers/spdk_endpoint_discoverer.hpp"
#include "discovery/builders/spdk_endpoint_builder.hpp"
#include "spdk/model/nvmf_subsystem.hpp"



using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework::discovery;
using namespace agent::spdk;
using namespace agent::spdk::discovery;

namespace {

void read_connected_entities(AgentContext::SPtr context,
                             const std::vector<::spdk::model::Namespace>& nvmf_namespaces,
                             Endpoint& target_endpoint) {

    if (nvmf_namespaces.empty()) {
        target_endpoint.set_status({enums::State::Enabled, enums::Health::Warning});
        return;
    }

    for (const auto& nvmf_namespace : nvmf_namespaces) {
        std::vector<::spdk::model::Bdev> bdevs{};
        auto base_bdev_uuid = nvmf_namespace.get_bdev_name(); // There is UUID of a volume (not alias!)

        try {
            bdevs = context->spdk_api->get_bdevs(base_bdev_uuid);
        }
        catch (const std::exception& exception) {
            log_error("spdk-discovery", "Failed to call get_bdevs method on SPDK: " << exception.what());
        }

        if (!bdevs.empty()) {
            auto base_bdev = bdevs.front();

            auto volumes = get_manager<Volume>().get_entries([&base_bdev_uuid](const Volume& volume) -> bool {
                return volume.get_name() == base_bdev_uuid;
            });

            for (const auto& volume : volumes) {
                SpdkEndpointBuilder::add_target_connected_entity(target_endpoint, volume.get_uuid());
            }
        }
        else {
            target_endpoint.set_status({enums::State::Enabled, enums::Health::Warning});
        }
    }
}

void read_transport_details(const std::vector<::spdk::model::ListenAddress>& nvmf_listen_addresses,
                            Endpoint& target_endpoint) {

    if (nvmf_listen_addresses.empty()) {
        target_endpoint.set_status({enums::State::UnavailableOffline, enums::Health::Critical});
        return;
    }

    for (const auto& listen_address : nvmf_listen_addresses) {
        if (listen_address.get_trtype() != ::spdk::model::TransportType::RDMA) {
            log_warning("spdk-discovery", "Unsupported transport type ("
                << listen_address.get_trtype() << ") for endpoint " << target_endpoint.get_uuid());
            continue;
        }

        if (listen_address.get_adrfam() != ::spdk::model::AddressFamily::IPv4) {
            log_warning("spdk-discovery", "Unsupported address family ("
                << listen_address.get_adrfam().value_or_default() << ") for endpoint " << target_endpoint.get_uuid());
            continue;
        }

        auto interfaces = agent_framework::module::get_manager<NetworkInterface>()
            .get_keys([&listen_address](const NetworkInterface& interface) -> bool {
                for (const auto& ipv4_address : interface.get_ipv4_addresses()) {
                    if (ipv4_address.get_address() == listen_address.get_traddr()) {
                        return true;
                    }
                }
                return false;
            });

        SpdkEndpointBuilder::add_ip_transport_details(target_endpoint, listen_address, interfaces);
    }
}

}


std::vector<Endpoint> SpdkEndpointDiscoverer::discover(const Uuid& parent_uuid) {
    std::vector<Endpoint> endpoints{};
    std::vector<::spdk::model::NvmfSubsystem> nvmf_subsystems{};

    try {
        nvmf_subsystems = m_context->spdk_api->get_nvmf_subsystems();
    }
    catch (const std::exception& exception) {
        log_error("spdk-discovery", "Failed to call get_nvmf_subsystem method on SPDK: " << exception.what());
    }

    for (const auto& nvmf_subsystem : nvmf_subsystems) {
        log_debug("spdk-discovery", "\n" << nvmf_subsystem.to_string());
        if (nvmf_subsystem.get_subtype() == ::spdk::model::NvmfSubsystem::DISCOVERY) {
            continue;
        }

        try {
            auto target_endpoint = SpdkEndpointBuilder::build_default(parent_uuid);
            SpdkEndpointBuilder::update_target_with_spdk(target_endpoint, nvmf_subsystem);
            ::read_connected_entities(m_context, nvmf_subsystem.get_namespaces(), target_endpoint);
            ::read_transport_details(nvmf_subsystem.get_listen_addresses(), target_endpoint);

            // TODO: save all hosts from HW to DB

            endpoints.emplace_back(std::move(target_endpoint));
        }
        catch (const std::exception& exception) {
            log_error("spdk-discovery",
                      "Failed to convert endpoint model from SPDK NVMf Subsystem: " << exception.what());
        }
    }

    return endpoints;
}

std::vector<Endpoint> SpdkEndpointDiscoverer::discovery_database(const Uuid& parent_uuid) {
    std::vector<Endpoint> endpoints{};
    for (const auto& entity : agent_framework::database::EndpointEntity::get_aggregates()) {
        try {
            agent_framework::database::EndpointEntity db{entity.get_uuid()};

            auto endpoint = SpdkEndpointBuilder::build_default(parent_uuid);
            endpoint.set_uuid(entity.get_uuid());
            SpdkEndpointBuilder::nvmeof_protocol(endpoint);

            auto nqn = db.get(agent_framework::database::ENDPOINT_NQN_PROPERTY);
            IdentifierBuilder::set_nqn(endpoint, nqn);
            auto role = db.get(agent_framework::database::ENDPOINT_ROLE_PROPERTY);
            if (enums::EntityRole::from_string(role) == enums::EntityRole::Initiator) {
                SpdkEndpointBuilder::add_initiator_connected_entity(endpoint);
                endpoints.push_back(std::move(endpoint));
            }
            else {
                log_error("spdk-discovery", "Unsupported endpoint role (" << role
                    << ") read from database for UUID '" << entity.get_uuid() << "' NQN '" << nqn << "'");
            }
        }
        catch (const std::exception& exception) {
            log_error("spdk-discovery", "Reading endpoint from DB failed: " << exception.what());
        }
    }

    return endpoints;
}
