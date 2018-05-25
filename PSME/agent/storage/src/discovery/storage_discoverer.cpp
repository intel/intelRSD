/*!
 * @brief Storage discoverer implementation.
 *
 * @header{License}
 * @copyright Copyright (c) 2017-2018 Intel Corporation.
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
 * @header{Filesystem}
 * @file storage_discoverer.cpp
 */


#include "discovery/storage_discoverer.hpp"
#include "discovery/builders/drive_builder.hpp"
#include "discovery/builders/fabric_builder.hpp"
#include "discovery/builders/zone_builder.hpp"
#include "discovery/builders/system_builder.hpp"
#include "discovery/builders/endpoint_builder.hpp"
#include "discovery/builders/ethernet_interface_builder.hpp"
#include "database/persistent_attributes.hpp"
#include "database/aggregate.hpp"

#include "agent-framework/version.hpp"
#include "agent-framework/module/common_components.hpp"

#include "net/network_interface.hpp"
#include "sysfs/sysfs_interface.hpp"


using namespace agent_framework;
using namespace agent_framework::model;
using namespace agent_framework::module::managers;
using namespace agent::storage;
using namespace database;

namespace agent {
namespace storage {
namespace discovery {

StorageDiscoverer::~StorageDiscoverer() {}


bool StorageDiscoverer::discover_manager(model::Manager& manager) {
    manager.set_firmware_version(generic::Version::VERSION_STRING);
    return true;
}


bool StorageDiscoverer::discover_storage_service(agent_framework::model::StorageService&) {
    return true;
}


bool StorageDiscoverer::discover_chassis(agent_framework::model::Chassis&) {
    return true;
}


bool StorageDiscoverer::discover_system(agent_framework::model::System& system) {
    static constexpr const char GUID_PATH[] = "/sys/class/dmi/id/product_uuid";
    try {
        auto sysfs = std::make_shared<sysfs::SysfsInterface>();
        auto guid_file = sysfs->get_file(GUID_PATH);
        SystemBuilder::update_with_guid(system, guid_file);
        return system.get_guid().has_value();
    }
    catch(const std::exception& exception) {
        log_error("storage-discovery", "Could not read system GUID from sysfs: " << exception.what());
    }
    return false;
}


bool StorageDiscoverer::discover_ethernet_interfaces(std::vector<agent_framework::model::NetworkInterface>& interfaces,
                                                     const Uuid& parent_uuid) {

    auto& target_manager = agent_framework::module::get_manager<IscsiTarget, IscsiTargetManager>();
    auto& iscsi_data = target_manager.get_iscsi_data();

    const auto ifaces = net::NetworkInterface::get_interfaces();
    for (const auto& ifa : ifaces) {
        log_debug("storage-discovery", "Found interface " << ifa.get_name());

        if (ifa.get_name() == iscsi_data.get_portal_interface()) {
            auto interface = EthernetInterfaceBuilder::build_default(parent_uuid);
            EthernetInterfaceBuilder::update_with_net(interface, ifa);
            interfaces.emplace_back(std::move(interface));
        }
    }
    return true;
}


bool StorageDiscoverer::discover_fabric(agent_framework::model::Fabric&) {
    return true;
}


bool StorageDiscoverer::discover_drives(std::vector<agent_framework::model::Drive>& drives, const Uuid& parent_uuid) {

    auto bd_drives = SysfsApi::get_instance()->get_hard_drives();

    for (const auto& bd_drive : bd_drives) {
        auto drive = DriveBuilder::build_default(parent_uuid);
        DriveBuilder::update_with_sysfs(drive, bd_drive);
        DriveBuilder::add_identifier_with_uuid(drive);
        DriveBuilder::add_identifier_with_system_path(drive, bd_drive);
        drives.emplace_back(std::move(drive));
    }
    return true;
}


bool StorageDiscoverer::discover_zones(std::vector<agent_framework::model::Zone>& zones, const Uuid& parent_uuid) {

    auto& pa = PersistentAttributes::get_instance();
    auto& zone_endpoint_manager = agent_framework::module::get_m2m_manager<Zone, Endpoint>();

    for (const auto& record : Aggregate<PersistentAttributes>::get_aggregates(pa, literals::Zone::ZONE)) {
        auto zone = ZoneBuilder::build_default(parent_uuid);
        zone.set_uuid(record.get_uuid());

        try {
            for (const auto& endpoint_uuid : record.get_multiple_values(literals::Endpoint::ENDPOINT)) {
                try {
                    agent_framework::module::get_manager<Endpoint>().get_entry(endpoint_uuid);
                    zone_endpoint_manager.add_entry(record.get_uuid(), endpoint_uuid);
                }
                catch (const agent_framework::exceptions::InvalidUuid&) {
                    log_warning("storage-discovery", "Zone " << record.get_uuid() << " points to a missing Endpoint "
                                                             << endpoint_uuid);
                    zone.set_status({enums::State::Disabled, enums::Health::Critical});
                }
            }
        }
        catch (const std::out_of_range&) {
            log_debug("storage-discovery", "Zone " << record.get_uuid() << " has no Endpoints");
        }
        zones.emplace_back(std::move(zone));
    }

    return true;
}


bool StorageDiscoverer::discover_endpoints(std::vector<agent_framework::model::Endpoint>& endpoints,
                                           const Uuid& parent_uuid) {

    auto& pa = PersistentAttributes::get_instance();

    for (const auto& record : Aggregate<PersistentAttributes>::get_aggregates(pa, literals::Endpoint::ENDPOINT)) {
        auto endpoint = EndpointBuilder::build_default(parent_uuid);

        try {
            endpoint.set_uuid(record.get_uuid());

            attribute::Identifier uuid{};
            uuid.set_durable_name(record.get_uuid());
            uuid.set_durable_name_format(enums::IdentifierType::UUID);
            endpoint.add_identifier(std::move(uuid));

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
                Aggregate<PersistentAttributes> entity_data(pa, entity_uuid, literals::ConnectedEntity::ENTITY);

                attribute::ConnectedEntity entity{};
                entity.set_entity_role(
                    enums::EntityRole::from_string(entity_data.get(literals::ConnectedEntity::ROLE)));
                if (entity.get_entity_role() == enums::EntityRole::Target) {
                    is_target = true;
                    try {
                        attribute::Identifier lun{};
                        lun.set_durable_name_format(enums::IdentifierType::LUN);
                        lun.set_durable_name(entity_data.get(literals::ConnectedEntity::IDENTIFIERS));
                        entity.add_identifier(std::move(lun));
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

            const auto& iscsi_data = agent_framework::module::get_manager<IscsiTarget, IscsiTargetManager>().get_iscsi_data();
            auto interfaces = agent_framework::module::get_manager<NetworkInterface>()
                .get_keys([&iscsi_data](const NetworkInterface interface) {
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

    return true;
}

}
}
}
