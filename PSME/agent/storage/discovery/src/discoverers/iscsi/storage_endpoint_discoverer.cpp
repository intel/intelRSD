/*!
 * @brief Endpoint discoverer implementation.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License") override;
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file storage_endpoint_discoverer.cpp
 */


#include "discovery/discoverers/storage_endpoint_discoverer.hpp"
#include "agent/utils/iscsi/iscsi_errors.hpp"
#include "agent/utils/iscsi/utils.hpp"

#include "agent-framework/discovery/builders/identifier_builder.hpp"
#include "agent-framework/discovery/builders/endpoint_builder.hpp"
#include "agent-framework/database/database_entity.hpp"

#include "agent/utils/utils.hpp"

#include "tgt/target_parser.hpp"

#include "sysfs/construct_dev_path.hpp"



using namespace agent::storage;
using namespace agent::storage::discovery;
using namespace agent_framework::model;

namespace {

void discover_iscsi_targets(AgentContext::SPtr context) {
    tgt::Manager manager{context->configuration->get_tgt_socket()};
    auto response = manager.show_targets();
    if (!response.is_valid()) {
        throw std::runtime_error("Invalid iSCSI show target response: " +
                                 agent::storage::utils::IscsiErrors::get_error_message(response.get_error()));
    }
    tgt::TargetParser parser{};
    if (response.get_extra_data().empty()) {
        log_info("storage-discovery", "No iSCSI Targets returned from tgt daemon");
        return;
    }
    std::string iscsi_text(response.get_extra_data().data());
    if ((iscsi_text.size() + 1) < response.get_extra_data().size()) {
        log_warning("storage-discovery", "show targets response size: " << response.get_extra_data().size()
                                                                        << " truncated to: "
                                                                        << (iscsi_text.size() + 1));
    }
    auto targets = parser.parse(iscsi_text);
    for (const auto& target : targets) {
        /* Add target ID for iSCSI model */
        context->configuration->add_iscsi_target(target.get_target_id(), target);
    }
}


bool match_target_iqn(const tgt::TargetData& target, const Endpoint& endpoint) {
    for (const auto& identifier : endpoint.get_identifiers()) {
        if (identifier.get_durable_name_format() == enums::IdentifierType::iQN) {
            if (identifier.get_durable_name() == target.get_target_iqn()) {
                return true;
            }
        }
    }
    return false;
}


bool match_target_volumes(const tgt::LunDataCollection luns, const Endpoint::ConnectedEntities& entities) {
    if (luns.size() != entities.get_array().size()) {
        log_debug("storage-discovery", "The number of LUNs and Connected Entities differ");
        return false;
    }
    uint32_t entities_matched = 0;
    for (const auto& entity : entities.get_array()) {
        for (const auto& lun : luns) {
            std::uint64_t entity_lun{};
            if (entity.get_lun().has_value()) {
                entity_lun = entity.get_lun();
            }
            else {
                continue;
            }

            if ((uint64_t) lun->get_lun() == entity_lun) {
                std::string entity_device_path{};
                try {
                    auto volume = agent_framework::module::get_manager<Volume>().get_entry(entity.get_entity());

                    auto storage_pools = agent_framework::module::get_m2m_manager<StoragePool, Volume>().get_parents(
                        volume.get_uuid());

                    if (storage_pools.size() > 0) {
                        auto storage_pool = agent_framework::module::get_manager<StoragePool>().get_entry(
                            storage_pools.front());

                        entity_device_path = sysfs::construct_dev_path(storage_pool.get_name(), volume.get_name());
                    }
                    else {
                        log_debug("storage-discovery", "No storage pool assigned for volume " << volume.get_uuid());
                        return false;
                    }
                }
                catch (const std::exception&) {
                    log_debug("storage-discovery",
                              "Failed to load connected Volume (" << entity.get_entity() << ") device path.");
                    return false;
                }

                if (lun->get_device_path() == entity_device_path) {
                    entities_matched++;
                }
                else {
                    log_debug("storage-discovery",
                              "Could not match Endpoint's Entity: '" << entity.get_entity()
                                                                     << "', with iSCSI Target backing store for LUN: "
                                                                     << lun->get_lun());
                    return false;
                }
            }

        }
    }

    if (entities_matched != entities.get_array().size()) {
        log_debug("storage-discovery", "Could not match all LUNs to Endpoint's Entities");
        return false;
    }
    return true;
}


bool match_target_credentials(const tgt::TargetData& iscsi_target, const Endpoint& target_endpoint) {
    bool check_oneway = false;
    bool check_mutual = false;
    bool oneway_match = false;
    bool mutual_match = false;

    if (!iscsi_target.get_chap_username().empty()) {
        check_oneway = true;
    }
    if (!iscsi_target.get_mutual_chap_username().empty()) {
        check_mutual = true;
    }

    auto& zone_endpoint_manager = agent_framework::module::get_m2m_manager<Zone, Endpoint>();
    auto& endpoint_manager = agent_framework::module::get_manager<Endpoint>();

    if (zone_endpoint_manager.get_parents(target_endpoint.get_uuid()).empty()) {
        log_debug("storage-discovery", "Endpoint does not belong to any Zone: " << target_endpoint.get_uuid());
        return false;
    }

    if (check_mutual) {
        if (iscsi_target.get_mutual_chap_username() == target_endpoint.get_username()) {
            mutual_match = true;
        }
        else {
            log_debug("storage-discovery", "Could not match Mutual CHAP username");
            return false;
        }
    }
    else {
        if (target_endpoint.get_username().has_value()) {
            log_debug("storage-discovery", "Mutual CHAP credentials missing in iSCSI Target from Endpoint: "
                << target_endpoint.get_uuid());
            return false;
        }
    }

    const auto zone_uuid = zone_endpoint_manager.get_parents(target_endpoint.get_uuid()).front();
    for (const auto& child_endpoint_uuid : zone_endpoint_manager.get_children(zone_uuid)) {
        const auto& child_endpoint = endpoint_manager.get_entry(child_endpoint_uuid);
        for (const auto& entity : child_endpoint.get_connected_entities().get_array()) {
            if (entity.get_entity_role() == enums::EntityRole::Initiator) {
                if (check_oneway) {
                    if (iscsi_target.get_chap_username() == child_endpoint.get_username()) {
                        oneway_match = true;
                    }
                    else {
                        log_debug("storage-discovery", "Could not match One-way CHAP username");
                        return false;
                    }
                }
                else {
                    if (child_endpoint.get_username().has_value()) {
                        log_debug("storage-discovery",
                                  "One-way CHAP credentials missing in iSCSI Target from Endpoint: "
                                      << target_endpoint.get_uuid());
                        return false;
                    }
                }
                break; // In Zone can only be one initiator
            }
        }
    }

    if (check_oneway != oneway_match) {
        log_debug("storage-discovery", "Could not match One-way CHAP username");
        return false;
    }
    if (check_mutual != mutual_match) {
        log_debug("storage-discovery", "Could not match Mutual CHAP username");
        return false;
    }

    return true;
}


bool match_target_initiator(const tgt::TargetData& iscsi_target, const Endpoint& endpoint) {

    bool initiator_found = false; // Zone must have one Initiator Endpoint to match iSCSI Target

    auto& zone_endpoint_manager = agent_framework::module::get_m2m_manager<Zone, Endpoint>();
    auto& endpoint_manager = agent_framework::module::get_manager<Endpoint>();

    if (zone_endpoint_manager.get_parents(endpoint.get_uuid()).empty()) {
        log_debug("storage-discovery", "Endpoint does not belong to any Zone: " << endpoint.get_uuid());
        return false;
    }

    // Endpoint can be only in one Zone
    const Uuid zone_uuid = zone_endpoint_manager.get_parents(endpoint.get_uuid()).front();
    for (const auto& child_endpoint_uuid : zone_endpoint_manager.get_children(zone_uuid)) {
        const auto& child_endpoint = endpoint_manager.get_entry(child_endpoint_uuid);
        for (const auto& entity : child_endpoint.get_connected_entities().get_array()) {
            if (entity.get_entity_role() == enums::EntityRole::Initiator) {
                initiator_found = true;
                for (const auto& identifier : child_endpoint.get_identifiers()) {
                    if (identifier.get_durable_name_format() == enums::IdentifierType::iQN) {
                        std::string initiator_iqn{};
                        // Empty string for Initiator IQN is a valid value
                        initiator_iqn = iscsi_target.get_target_initiator();

                        // Limited to one initiator IQN per Initiator Endpoint - if IQN exists it must match
                        if (identifier.get_durable_name() != initiator_iqn) {
                            log_debug("storage-discovery", "Endpoint IQN: '"
                                << identifier.get_durable_name() << "' and iSCSI Target initiator: '"
                                << initiator_iqn << "' do not match");
                            return false;
                        }
                    }
                }
            }
            break; // Zone can only have one Initiator Endpoint, so when it is found the loop can be broken
        }
    }

    if (!initiator_found) {
        log_debug("storage-discovery", "Could not find Initiator Endpoint in the Zone");
        return false;
    }
    return true;
}


void remove_duplicates(std::vector<std::string>& to_have_matching_values_removed, std::vector<std::string>& values) {
    to_have_matching_values_removed.erase(
        std::remove_if(std::begin(to_have_matching_values_removed),
                       std::end(to_have_matching_values_removed),
                       [&](std::string x) {
                           return find(begin(values), end(values), x) != end(values);
                       }),
        std::end(to_have_matching_values_removed));
}


void clean_chap_from_connected_endpoints(const Uuid& endpoint_uuid) {
    auto endpoint = agent_framework::module::get_manager<Endpoint>().get_entry_reference(endpoint_uuid);
    endpoint->clean_username();
    agent::storage::utils::clean_chap_from_database(endpoint_uuid);

    const auto zone_uuid = agent_framework::module::get_m2m_manager<Zone, Endpoint>().get_parents(
        endpoint_uuid).front();
    for (const auto& child_endpoint_uuid : agent_framework::module::get_m2m_manager<Zone, Endpoint>().get_children(
        zone_uuid)) {
        auto child_endpoint = agent_framework::module::get_manager<Endpoint>().get_entry_reference(child_endpoint_uuid);
        for (const auto& entity : child_endpoint->get_connected_entities().get_array()) {
            if (entity.get_entity_role() == enums::EntityRole::Initiator) {
                child_endpoint->clean_username();
                agent::storage::utils::clean_chap_from_database(child_endpoint_uuid);
            }
        }
    }
}

}


std::vector<Endpoint> StorageEndpointDiscoverer::discover(const Uuid& parent_uuid) {
    std::vector<Endpoint> endpoints{};
    auto& pa = database::PersistentAttributes::get_instance();

    for (const auto& record : database::Aggregate<database::PersistentAttributes>::get_aggregates(pa,
                                                                                                  literals::Endpoint::ENDPOINT)) {
        auto endpoint = agent_framework::discovery::EndpointBuilder::build_default(parent_uuid);
        agent_framework::discovery::EndpointBuilder::iscsi_protocol(endpoint);

        try {
            endpoint.set_uuid(record.get_uuid());

            attribute::Identifier iqn{};
            iqn.set_durable_name_format(enums::IdentifierType::iQN);
            iqn.set_durable_name(record.get(literals::Identifier::DURABLE_NAME));
            endpoint.add_identifier(std::move(iqn));
        }
        catch (const std::out_of_range& e) {
            log_warning("storage-discovery", "Could not read Endpoint's Identifiers from the database: " << e.what());
            continue;
        }

        try {
            if (!record.get(literals::Endpoint::USERNAME).empty()) {
                endpoint.set_username(record.get(literals::Endpoint::USERNAME));
            }
        }
        catch (const std::out_of_range&) {
            log_debug("storage-discovery", "Username not set for Endpoint " << record.get_uuid());
        }

        bool is_target = false;
        for (const auto& entity_uuid : record.get_multiple_values(literals::ConnectedEntity::ENTITY)) {
            try {
                database::Aggregate<database::PersistentAttributes> entity_data(pa, entity_uuid,
                                                                                literals::ConnectedEntity::ENTITY);

                attribute::ConnectedEntity entity{};
                entity.set_entity_role(
                    enums::EntityRole::from_string(entity_data.get(literals::ConnectedEntity::ROLE)));
                if (entity.get_entity_role() == enums::EntityRole::Target) {
                    is_target = true;
                    try {
                        std::int64_t lun = std::stoi(entity_data.get(literals::ConnectedEntity::IDENTIFIERS));
                        entity.set_lun(lun);

                        entity.set_entity(entity_data.get(literals::ConnectedEntity::ENTITY));
                        agent_framework::module::get_manager<Volume>().get_entry(
                            entity_data.get(literals::ConnectedEntity::ENTITY));
                    }
                    catch (const agent_framework::exceptions::InvalidUuid&) {
                        log_warning("storage-discovery",
                                    "Endpoint " << record.get_uuid() << " points to a missing Volume "
                                                << entity_data.get(literals::ConnectedEntity::ENTITY));
                        endpoint.set_status({enums::State::Disabled, enums::Health::Critical});
                    }
                }
                endpoint.add_connected_entity(std::move(entity));
            }
            catch (const std::out_of_range& e) {
                log_debug("storage-discovery",
                          "Could not read Connected Entity for Endpoint " << record.get_uuid() << ": " << e.what());
                continue;
            }
        }

        if (is_target) {
            attribute::IpTransportDetail ip_transport_detail{};
            ip_transport_detail.set_transport_protocol(enums::TransportProtocol::iSCSI);

            const auto& iscsi_data = m_context->configuration->get_iscsi_data();
            auto interfaces = agent_framework::module::get_manager<NetworkInterface>()
                .get_keys([&iscsi_data](const NetworkInterface& interface) {
                    return interface.get_name() == iscsi_data.get_portal_interface();
                });

            ip_transport_detail.set_ipv4_address({iscsi_data.get_portal_ip()});
            ip_transport_detail.set_port(iscsi_data.get_portal_port());

            if (!interfaces.empty()) {
                ip_transport_detail.set_interface(interfaces.front());
            }
            else {
                log_warning("storage-agent", "Cannot find iSCSI portal interface "
                    << iscsi_data.get_portal_interface()
                    << " in the system interfaces for endpoint: "
                    << record.get_uuid());
            }

            endpoint.add_ip_transport_detail(std::move(ip_transport_detail));
        }

        endpoints.emplace_back(std::move(endpoint));
    }

    return endpoints;

}


void StorageEndpointDiscoverer::restore() const {
    // Storage Agent deletes from tgt iSCSI Targets which do not match any existing Zone/Endpoint,
    // and adds to tgt iSCSI Targets based on existing Zones/Endpoints.
    ::discover_iscsi_targets(m_context);

    std::vector<tgt::TargetData::Id> iscsi_targets_for_removal{};
    std::vector<std::string> endpoints_matched{};

    for (const auto& target_entry : m_context->configuration->get_iscsi_targets()) {
        const auto& iscsi_target = target_entry.second;
        bool iscsi_target_remove = true;
        for (const auto& endpoint : agent_framework::module::get_manager<Endpoint>().get_entries()) {
            if (::match_target_iqn(iscsi_target, endpoint)) {
                log_debug("storage-discovery",
                          "iSCSI Target name matched with Endpoint IQN: " << iscsi_target.get_target_iqn());

                if (::match_target_volumes(iscsi_target.get_luns(), endpoint.get_connected_entities())) {
                    if (::match_target_initiator(iscsi_target, endpoint)) {
                        if (::match_target_credentials(iscsi_target, endpoint)) {
                            log_debug("storage-discovery", "Successfully matched existing iSCSI Target: '"
                                << iscsi_target.get_target_iqn()
                                << "' and Target Endpoint: "
                                << endpoint.get_uuid());
                            iscsi_target_remove = false;
                            endpoints_matched.emplace_back(endpoint.get_uuid());
                        }
                        else {
                            log_debug("storage-discovery", "Partially matched existing iSCSI Target: '"
                                << iscsi_target.get_target_iqn()
                                << "' and Target Endpoint: "
                                << endpoint.get_uuid());
                        }
                    }
                    else {
                        log_debug("storage-discovery", "Could not match iSCSI Target initiator IQN with Endpoint: "
                            << iscsi_target.get_target_iqn());
                    }
                }
                else {
                    log_debug("storage-discovery", "Could not match iSCSI Target LUNs with any Endpoint's Entities: "
                        << iscsi_target.get_target_iqn());
                }
                break;
            }
        }
        if (iscsi_target_remove) {
            iscsi_targets_for_removal.emplace_back(target_entry.first);
        }
    }

    for (const auto& target_id_to_remove : iscsi_targets_for_removal) {
        log_debug("storage-discovery", "iSCSI Target which will be removed from tgt: " << target_id_to_remove);
        try {
            agent::storage::utils::remove_iscsi_target(m_context, target_id_to_remove);
        }
        catch (const agent_framework::exceptions::IscsiError& e) {
            log_debug("storage-discovery", "Error while removing iSCSI Target: " << e.get_message());
        }
    }

    std::vector<std::string> unmatched_endpoints = agent_framework::module::get_manager<Endpoint>().get_keys();
    // Removes Endpoints matched with existing iSCSI Targets from the list of all Endpoints from the database
    ::remove_duplicates(unmatched_endpoints, endpoints_matched);

    for (const auto& endpoint_uuid : unmatched_endpoints) {
        if (agent::storage::utils::is_endpoint_valid_for_iscsi(endpoint_uuid)) {
            if (agent::storage::utils::is_endpoint_valid_for_chap(m_context, endpoint_uuid)) {
                log_debug("storage-discovery", "iSCSI Target which will be added to tgt: " << endpoint_uuid);
            }
            else {
                log_debug("storage-discovery",
                          "iSCSI Target which will be added to tgt without CHAP: " << endpoint_uuid);
                clean_chap_from_connected_endpoints(endpoint_uuid);
            }
            agent::storage::utils::add_iscsi_target(m_context, endpoint_uuid, true);
        }
        else {
            log_debug("storage-discovery",
                      "Endpoint is not valid to create an iSCSI Target: " << endpoint_uuid);
            // If Endpoint's username is not in use in TGT, then credentials cannot be restored
            if (!agent::storage::utils::endpoint_represents_iscsi_target(endpoint_uuid)) {
                agent::storage::utils::clean_chap_from_endpoint(endpoint_uuid);
            }
        }
    }
}
