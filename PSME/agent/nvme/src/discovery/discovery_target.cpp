/*!
 * @brief Implementation of discovery target.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file discovery_target.cpp
 */

#include "discovery/discovery_target.hpp"
#include "tree_stability/nvme_stabilizer.hpp"
#include "tools/endpoint_reader.hpp"
#include "tools/endpoint_creator.hpp"
#include "tools/udev.hpp"
#include "tools/nvme_context.hpp"
#include "tools/databases.hpp"
#include "tools/tools.hpp"
#include "loader/config.hpp"
#include "logger/logger_factory.hpp"
#include "netlink/utils.hpp"

#include "agent-framework/module/common_components.hpp"

#include <net/if.h>

#include "sysfs/construct_dev_path.hpp"



using namespace agent::nvme::discovery;
using namespace agent::nvme::loader;
using namespace agent::nvme::tools;
using namespace agent::nvme;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework::model::enums;

namespace {

void fill_drive_data(Drive& drive, const tools::BaseDriveHandler::DriveData& dd) {

    if (dd.block_size_bytes.has_value()) {
        drive.set_block_size_bytes(dd.block_size_bytes);
        if (dd.size_lba.has_value()) {
            drive.set_capacity_gb(double(dd.size_lba * dd.block_size_bytes) / 1000.0 / 1000.0 / 1000.0);
        }
    }

    attribute::FruInfo fru_info{};
    if (dd.serial_number.has_value()) {
        fru_info.set_serial_number(dd.serial_number);
    }
    if (dd.model_number.has_value()) {
        fru_info.set_model_number(dd.model_number);
    }
    if (dd.manufacturer_number.has_value()) {
        fru_info.set_manufacturer(dd.manufacturer_number);
    }
    drive.set_fru_info(std::move(fru_info));

    if (dd.firmware_revision.has_value()) {
        drive.set_firmware_version(dd.firmware_revision);
    }

    if (dd.features.latency_tracking_enabled.has_value()) {
        drive.set_latency_tracking_enabled(dd.features.latency_tracking_enabled);
    }

}


void fill_volume_data(Volume& volume, const BaseDriveHandler::VolumeData& vd,
                      const Uuid& pool_uuid, std::uint64_t pool_size) {

    if (vd.block_size_bytes.has_value()) {
        volume.set_block_size_bytes(vd.block_size_bytes);
        if (vd.size_lba.has_value()) {
            attribute::Capacity cap{};
            cap.set_allocated_bytes(vd.size_lba * vd.block_size_bytes);
            volume.set_capacity(std::move(cap));
        }
    }
    if (!pool_uuid.empty()) {
        attribute::CapacitySource caps{};
        caps.set_allocated_bytes(pool_size);
        caps.add_providing_pool(pool_uuid);
        volume.set_capacity_sources({caps});
    }

    volume.add_identifier({vd.uuid, enums::IdentifierType::UUID});
}


void print_drive_log(const Drive& drive) {
    log_debug("nvme-agent", "\tModel: " << drive.get_fru_info().get_model_number());
    log_debug("nvme-agent", "\tManufacturer: " << drive.get_fru_info().get_manufacturer());
    log_debug("nvme-agent", "\tSerial number: " << drive.get_fru_info().get_serial_number());
    log_debug("nvme-agent", "\tFirmware version: " << drive.get_firmware_version());
    log_debug("nvme-agent", "\tDisk size (GB): " << drive.get_capacity_gb());
    log_debug("nvme-agent", "\tBlock size (bytes): " << drive.get_block_size_bytes());
}


void print_volume_log(const Volume& volume) {
    log_debug("nvme-agent", "\tBlock size (bytes): " << volume.get_block_size_bytes());
    log_debug("nvme-agent", "\tVolume size (bytes): " << volume.get_capacity().get_allocated_bytes());
    log_debug("nvme-agent", "\tVolume GUID: " << attribute::Identifier::get_uuid(volume));
}

}


DiscoveryTarget::DiscoveryTarget(std::shared_ptr<NvmeAgentContext> context, BaseEndpointReader& endpoint_reader,
                                 BaseEndpointCreator& endpoint_creator) :
    m_endpoint_reader(endpoint_reader),
    m_endpoint_creator(endpoint_creator),
    m_context(context) {}


DiscoveryTarget::~DiscoveryTarget() {}


namespace {

void update_manager(const Uuid& manager_uuid) {
    auto manager = get_manager<Manager>().get_entry_reference(manager_uuid);
    manager->add_collection({CollectionName::StorageServices,
                             CollectionType::StorageServices});
}


void update_chassis(const Uuid& chassis_uuid) {
    auto chassis_ref = get_manager<Chassis>().get_entry_reference(chassis_uuid);
    ChassisDatabase db{chassis_uuid};
    try {
        std::string value = db.get(NvmeDatabase::CHASSIS_ASSET_TAG);
        log_debug("nvme-agent", "Loaded chassis asset tag: " << value);
        chassis_ref->set_asset_tag(value);
    }
    catch (const std::exception&) {
        log_debug("nvme-agent", "Asset Tag attribute is not stored for chassis");
    }
}


void discover_network_interface(const std::string& interface, const Uuid& parent) {
    NvmeStabilizer ns{};
    auto ifindex = if_nametoindex(interface.c_str());
    if (0 != ifindex) {
        NetworkInterface ni{parent};
        ni.set_status(attribute::Status(State::Enabled, Health::OK));
        auto mac_address = netlink_base::utils::get_port_mac_address(interface);
        if (!mac_address.empty()) {
            ni.set_mac_address(mac_address);
            ni.set_factory_mac_address(mac_address);
        }
        std::string ipv4{}, ipv4_mask{};
        ipv4 = netlink_base::utils::get_port_ip_address(ifindex, ipv4_mask);
        if (!ipv4.empty()) {
            attribute::Ipv4Address ipv4_address{};
            ipv4_address.set_address(ipv4);
            if (!ipv4_mask.empty()) {
                ipv4_address.set_subnet_mask(ipv4_mask);
            }
            ni.add_ipv4_address(ipv4_address);
        }
        ns.stabilize(ni);
        get_manager<NetworkInterface>().add_entry(ni);
    }
    else {
        log_error("nvme-agent", "Unable to get index for network interface " << interface);
    }
}


void discover_network_interfaces(const Uuid& parent) {
    UDev udev{};
    std::stringstream drivers{NvmeConfig::get_instance()->get_nic_drivers()};
    std::istream_iterator<std::string> it{drivers};
    std::istream_iterator<std::string> end{};
    while (it != end) {
        std::string driver_name{*(it++)};
        log_debug("nvme-agent", "Looking for network interfaces by driver " << driver_name);
        auto interfaces = udev.get_interface_list(driver_name, "");
        for (const auto& interface : interfaces) {
            log_debug("nvme-agent", "Discovered network interface " << interface);
            discover_network_interface(interface, parent);
        }
    }
}


Uuid create_storage_pool(const Uuid& storage_service_uuid, const Drive& drive, uint64_t drive_size_bytes) {
    NvmeStabilizer ns{};
    StoragePool storage_pool{storage_service_uuid};
    attribute::CapacitySource capacity_source{};
    attribute::Capacity capacity{};

    if (drive.get_name().has_value()) {
        storage_pool.set_name(drive.get_name());
    }
    else {
        log_error("nvme-agent", "Drive '" + drive.get_uuid()
                                + "' has no name. Storage pool was not created.");
        return {};
    }
    storage_pool.set_status({State::Enabled, Health::OK});
    if (drive.get_block_size_bytes().has_value()) {
        storage_pool.set_block_size_bytes(drive.get_block_size_bytes());
        capacity.set_allocated_bytes(drive_size_bytes);
        storage_pool.set_capacity(std::move(capacity));
    }

    capacity_source.add_providing_drive(drive.get_uuid());
    if (drive.get_block_size_bytes().has_value()) {
        capacity_source.set_allocated_bytes(drive_size_bytes);
        capacity_source.set_consumed_bytes(drive_size_bytes);
    }
    storage_pool.add_capacity_source(std::move(capacity_source));

    storage_pool.add_collection({CollectionName::Volumes, CollectionType::Volumes});
    ns.stabilize(storage_pool);

    attribute::Identifier uuid{};
    uuid.set_durable_name(storage_pool.get_uuid());
    uuid.set_durable_name_format(enums::IdentifierType::UUID);
    storage_pool.add_identifier(uuid);

    get_manager<StoragePool>().add_entry(storage_pool);
    return storage_pool.get_uuid();
}


void discover_volumes(std::shared_ptr<BaseDriveHandler> handler, const std::string& storage_service_uuid,
                      const std::string& storage_pool_uuid, uint64_t drive_size_bytes) {

    NvmeStabilizer ns{};
    log_debug("nvme-agent", "Discovering volumes... ");

    for (const auto& volume_name : handler->get_volumes()) {
        log_debug("nvme-agent", "Discovered volume " << volume_name);

        Volume volume{storage_service_uuid};
        volume.set_status(attribute::Status(State::Enabled, Health::OK));
        volume.set_name(volume_name);

        auto vd = handler->get_volume_data(volume_name);
        fill_volume_data(volume, vd, storage_pool_uuid, drive_size_bytes);
        print_volume_log(volume);

        ns.stabilize(volume);
        get_manager<Volume>().add_entry(volume);
        get_m2m_manager<StoragePool, Volume>().add_entry(storage_pool_uuid, volume.get_uuid());
    }
}


void
discover_drives(std::shared_ptr<NvmeAgentContext> context, const Uuid& chassis_uuid, const Uuid& storage_service_uuid) {
    NvmeStabilizer ns{};

    try {
        for (const auto& drive_name : context->drive_reader->get_drives()) {
            log_debug("nvme-agent", "Discovered drive " << drive_name);

            Drive drive{chassis_uuid};
            drive.set_status(attribute::Status(State::Enabled, Health::OK));
            drive.set_name(drive_name);
            drive.set_type(DriveType::SSD);
            drive.set_interface(TransportProtocol::NVMe);

            auto handler = context->drive_handler_factory->get_handler(drive_name);
            BaseDriveHandler::DriveData dd{};
            uint64_t available_size_bytes = 0;
            uint64_t size_bytes = 0;
            if (!handler) {
                log_error("nvme-agent", "No handler available for the drive " << drive_name);
            }
            else {
                handler->load();
                dd = handler->get_drive_data();
                if (dd.block_size_bytes.has_value() && dd.available_size_lba.has_value()) {
                    available_size_bytes = uint64_t(dd.block_size_bytes) * uint64_t(dd.available_size_lba);
                }
                if (dd.block_size_bytes.has_value() && dd.size_lba.has_value()) {
                    size_bytes = uint64_t(dd.block_size_bytes) * uint64_t(dd.size_lba);
                }
                fill_drive_data(drive, dd);
            }

            print_drive_log(drive);
            log_debug("nvme-agent", "\tTotal size, bytes:   " << size_bytes);
            log_debug("nvme-agent", "\tTotal size, blocks:  " << dd.size_lba);
            log_debug("nvme-agent", "\tUsable size, bytes:  " << available_size_bytes);
            log_debug("nvme-agent", "\tUsable size, blocks: " << dd.available_size_lba);

            ns.stabilize(drive);
            drive.add_identifier({drive.get_uuid(), enums::IdentifierType::UUID});

            get_manager<Drive>().add_entry(drive);
            get_m2m_manager<StorageService, Drive>().add_entry(storage_service_uuid, drive.get_uuid());
            auto storage_pool_uuid = create_storage_pool(storage_service_uuid, drive, available_size_bytes);

            if (handler) {
                discover_volumes(handler, storage_service_uuid, storage_pool_uuid, available_size_bytes);
                update_storage_pool_consumed_capacity(storage_pool_uuid);
            }

        }
    }
    catch (const std::runtime_error& error) {
        log_error("nvme-agent", "Unable to read NVMe drives: " << error.what());
    }
}


Uuid create_storage_service(const Uuid& manager_uuid) {
    NvmeStabilizer ns{};

    StorageService storage_service{manager_uuid};
    storage_service.set_status({State::Enabled, Health::OK});
    storage_service.add_collection({CollectionName::StoragePools, CollectionType::StoragePools});
    storage_service.add_collection({CollectionName::Volumes, CollectionType::Volumes});
    storage_service.add_collection({CollectionName::Drives, CollectionType::Drives});
    ns.stabilize(storage_service);
    get_manager<StorageService>().add_entry(storage_service);
    return storage_service.get_uuid();
}


void add_endpoint_to_zone(const Uuid& zone, const Uuid& endpoint) {
    if (!get_manager<Endpoint>().entry_exists(endpoint)) {
        log_error("nvme-agent", "No endpoint found in manager with given UUID '" + endpoint + "'");
        return;
    }
    get_m2m_manager<Zone, Endpoint>().add_entry(zone, endpoint);
}


void load_endpoints_from_zone_database(std::shared_ptr<NvmeAgentContext> context, const Uuid& zone) {
    ZoneDatabase zone_db(zone);
    auto endpoints{zone_db.get_multiple_values(NvmeDatabase::ENDPOINTS)};
    std::for_each(endpoints.begin(), endpoints.end(),
                  [&zone](const std::string& endpoint) { add_endpoint_to_zone(zone, endpoint); });
    set_initiator_filter(context, endpoints);
}


void add_zone(std::shared_ptr<NvmeAgentContext> context, const Uuid& fabric_uuid, const Uuid& zone_uuid) {
    Zone zone{fabric_uuid};
    zone.set_uuid(zone_uuid);

    zone.set_status(attribute::Status(State::Enabled, Health::OK));
    zone.add_collection(attribute::Collection(CollectionName::Endpoints, CollectionType::Endpoints));
    get_manager<Zone>().add_entry(zone);
    load_endpoints_from_zone_database(context, zone_uuid);
}


void discover_zones(std::shared_ptr<NvmeAgentContext> context, const Uuid& fabric) {
    FabricDatabase fabric_db(fabric);
    auto zones{fabric_db.get_multiple_values(NvmeDatabase::ZONES)};
    std::for_each(zones.begin(), zones.end(), [&](const std::string& zone) { ::add_zone(context, fabric, zone); });
}


void add_initiator(std::shared_ptr<NvmeAgentContext> context, const Uuid& fabric, const Uuid& uuid) {
    Endpoint endpoint{fabric};
    try {
        EndpointDatabase db{uuid};
        endpoint.add_identifier(
            {literals::Endpoint::NQN_FORMAT + db.get(EndpointReader::DATABASE_NQN), IdentifierType::NQN});
    }
    catch (const std::exception& e) {
        log_error("nvme-agent", "Unable to read initiator endpoint NQN from database: " << e.what());
        return;
    }
    endpoint.add_identifier({uuid, IdentifierType::UUID});
    endpoint.set_uuid(uuid);
    endpoint.set_status(attribute::Status(State::Enabled, Health::OK));
    attribute::ConnectedEntity ce{};
    ce.set_entity_role(EntityRole::Initiator);
    endpoint.add_connected_entity(ce);
    endpoint.set_protocol(TransportProtocol::NVMeOverFabrics);
    try {
        EndpointDatabase db{uuid};
        // add initiator nqn to hosts configuration
        context->nvme_target_handler->add_host(db.get(EndpointReader::DATABASE_NQN));
    }
    catch (std::exception& e) {
        // this is not a critical error, the link may already exist if the target configuration exists
        log_warning("nvme-agent", "Unable to add initiator to allowed hosts: " << e.what());
    }

    get_manager<Endpoint>().add_entry(endpoint);
}


Endpoint read_target(const Uuid& fabric, const Uuid& endpoint_uuid) {
    Endpoint endpoint{fabric};

    attribute::IpTransportDetail ip_transport_detail{};

    try {
        EndpointDatabase db{endpoint_uuid};
        endpoint.add_identifier({literals::Endpoint::NQN_FORMAT + db.get(EndpointReader::DATABASE_NQN),
                                 IdentifierType::NQN});
        ip_transport_detail.set_ipv4_address({db.get(EndpointReader::DATABASE_IPV4)});
        ip_transport_detail.set_port(std::stoi(db.get(EndpointReader::DATABASE_RDMA_PORT)));
        ip_transport_detail.set_transport_protocol(TransportProtocol::RoCEv2);
        endpoint.add_ip_transport_detail(ip_transport_detail);
        attribute::ConnectedEntity ce{};
        ce.set_entity_role(EntityRole::Target);
        ce.set_entity(db.get(EndpointReader::DATABASE_NVME_DEVICE_UUID));
        endpoint.add_connected_entity(ce);
    }
    catch (const std::exception& e) {
        log_error("nvme-discovery-agent", "Unable to read target endpoint from database: " << e.what());
        throw;
    }

    endpoint.add_identifier({endpoint_uuid, IdentifierType::UUID});
    endpoint.set_uuid(endpoint_uuid);
    endpoint.set_protocol(TransportProtocol::NVMeOverFabrics);
    endpoint.set_status(attribute::Status(State::Enabled, Health::OK));
    return endpoint;
}


auto endpoint_discovery_less = [](const Endpoint& lhs, const Endpoint& rhs) -> bool {
    if ((lhs.get_ip_transport_details().size() != 1) || (rhs.get_ip_transport_details().size() != 1)) {
        throw std::runtime_error("Unsupported ip transport details configuration for Endpoints");
    }

    if ((lhs.get_connected_entities().size() != 1) || (rhs.get_connected_entities().size() != 1)) {
        throw std::runtime_error("Unsupported connected entities configuration for Endpoints");
    }

    auto lhs_ip_details = lhs.get_ip_transport_details().front();
    auto rhs_ip_details = rhs.get_ip_transport_details().front();
    auto lhs_connected_entities = lhs.get_connected_entities().front();
    auto rhs_connected_entities = rhs.get_connected_entities().front();

    auto is_ip_less =
        (lhs_ip_details.get_ipv4_address().get_address() < rhs_ip_details.get_ipv4_address().get_address())
        || (lhs_ip_details.get_port() < rhs_ip_details.get_port())
        || (lhs_ip_details.get_transport_protocol() < rhs_ip_details.get_transport_protocol());

    auto is_ce_less = (lhs_connected_entities.get_entity_role() < rhs_connected_entities.get_entity_role())
                      || (lhs_connected_entities.get_entity() < rhs_connected_entities.get_entity());

    auto is_endpoint_less = (lhs.get_uuid() < rhs.get_uuid()) || (lhs.get_protocol() < rhs.get_protocol())
                            || (attribute::Identifier::get_nqn(lhs) < attribute::Identifier::get_nqn(rhs));

    return is_ip_less || is_ce_less || is_endpoint_less;
};


void recreate_endpoints(std::vector<Endpoint> database_targets, std::shared_ptr<NvmeAgentContext> context,
                        BaseEndpointReader& reader, BaseEndpointCreator& creator, const Uuid& fabric) {
    // export to a "rebuild_target_endpoints" function
    std::vector<Endpoint> to_be_created;
    auto hardware_targets = reader.read_endpoints(context, fabric);

    // std::set_difference requires both containers to be sorted
    std::sort(std::begin(database_targets), std::end(database_targets), endpoint_discovery_less);
    std::sort(std::begin(hardware_targets), std::end(hardware_targets), endpoint_discovery_less);

    // extract all db targets which were not found on the hw
    std::set_difference(std::begin(database_targets), std::end(database_targets), std::begin(hardware_targets),
                        std::end(hardware_targets), std::back_inserter(to_be_created), endpoint_discovery_less);

    for (auto& target : to_be_created) {
        try {
            auto nqn = attribute::Identifier::get_nqn(target);
            const auto& transport_details = target.get_ip_transport_details().front();
            convert_to_subnqn(nqn);
            // no need to check if target.get_connected_entities() is empty, the compare function used in sort does that
            const auto& volume = get_manager<Volume>().get_entry(
                target.get_connected_entities().front().get_entity().value());

            std::string volume_path{};
            if (volume.get_name().has_value()) {
                volume_path = sysfs::construct_dev_path(volume.get_name());
            }
            else {
                THROW(agent_framework::exceptions::NvmeError, "nvme-agent", "Volume's name must be known.");
            }

            creator.create_target_endpoint(context, transport_details.get_ipv4_address().get_address(),
                                           static_cast<uint16_t>(transport_details.get_port()), nqn,
                                           volume_path);

            get_manager<Endpoint>().add_entry(target);

        }
        catch (const std::exception& e) {
            log_error("nvme-agent", "Failed restoring endpoint " << target.get_uuid() << ": " << e.what());
        }
    }

    if (!to_be_created.empty() && to_be_created.size() != database_targets.size()) {
        log_warning("nvme-agent", "Host exposes a target not created through PSME API");
    }

    for (const auto& target : hardware_targets) {
        get_manager<Endpoint>().add_entry(target);
    }
}


void discover_endpoints(std::shared_ptr<NvmeAgentContext> context, BaseEndpointReader& reader,
                        BaseEndpointCreator& creator, const Uuid& fabric) {
    std::vector<Endpoint> database_targets;

    FabricDatabase fabric_db{fabric};
    auto endpoints = fabric_db.get_multiple_values(NvmeDatabase::ENDPOINTS);
    // filter out new endpoints?
    for (auto endpoint_uuid : endpoints) {
        try {
            EndpointDatabase db{endpoint_uuid};
            auto role = db.get(EndpointReader::DATABASE_ROLE);
            if (literals::Endpoint::TARGET == role) {
                database_targets.push_back(read_target(fabric, endpoint_uuid));
            }
            else if (literals::Endpoint::INITIATOR == role) {
                // initiators can be added automatically
                add_initiator(context, fabric, endpoint_uuid);
            }
            else { // INITIATOR
                log_error("nvme-discovery-agent", "Unrecognized endpoint role read from database " << role);
            }
        }
        catch (const std::exception&) {
            log_error("nvme-discovery-agent", "Unable to read endpoint");
        }
    }

    try {
        recreate_endpoints(database_targets, context, reader, creator, fabric);
    }
    catch (const std::exception& e) {
        log_error("nvme-agent", "Unable to recreate NVMe target endpoints."
            << " Make sure you have nvmet-rdma kernel module loaded. "
            << e.what());
    }
}

}


void DiscoveryTarget::discover(const Uuid& manager_uuid, const Uuid& fabric_uuid) {
    update_manager(manager_uuid);
    Uuid storage_service_uuid = ::create_storage_service(manager_uuid);
    for (const auto& system_uuid : get_manager<System>().get_keys()) {
        discover_network_interfaces(system_uuid);
        get_m2m_manager<System, StorageService>().add_entry(system_uuid, storage_service_uuid);
    }
    for (const auto& chassis_uuid : get_manager<Chassis>().get_keys()) {
        update_chassis(chassis_uuid);
        discover_drives(m_context, chassis_uuid, storage_service_uuid);
    }

    discover_endpoints(m_context, m_endpoint_reader, m_endpoint_creator, fabric_uuid);
    discover_zones(m_context, fabric_uuid);
}
