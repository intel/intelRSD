/*!
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
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
 * @file rmm/discovery/helpers/discover.hpp
 */

#pragma once



#include "discovery/helpers/common.hpp"
#include "utils/fru_reader.hpp"
#include "loader/ipmi_config.hpp"

#include "ipmi/command/sdv/rmm/get_pzone_presence.hpp"
#include "ipmi/command/sdv/rmm/get_psshelf_critical.hpp"
#include "ipmi/command/sdv/rmm/send_psu_command.hpp"
#include "ipmi/command/generic/get_lan_config_params.hpp"
#include "agent-framework/version.hpp"
#include "agent-framework/module/utils/is_ascii_string.hpp"


namespace agent {
namespace rmm {
namespace discovery {
namespace helpers {

/*!
 * @brief This is a generic discovery function for a resource. It is parametrized with the type of the resource
 * to be discovered. It accepts three arguments: parent resource uuid, discovery context that is meant
 * to provide access to ipmi controller and additional discovery parameters stored in discovery params struct.
 * It returns a newly discovered resource. Default implementation should never be called and generates
 * a compile time error.
 * */
template<RmmType TYPE>
ModelType<TYPE> discover(const std::string&, const DiscoveryContext&, const DiscoveryParams<TYPE>&) {
    static_assert(fail_assert<TYPE>(), "Discovery specialization required");
}

template<>
ModelType<RmmType::Psu> discover<RmmType::Psu>(const std::string& parent, const DiscoveryContext& dc,
                                               const DiscoveryParams<RmmType::Psu>& dp) {
    log_debug("rmm-discovery", "Psu discovery");
    ModelType<RmmType::Psu> res(parent);
    res.set_status(attribute::Status(enums::State::Enabled, enums::Health::OK));
    res.set_slot_id(dp.slot_id);

    auto model = call_psu_command<ipmi::command::sdv::request::SendPsuReadModel,
        ipmi::command::sdv::response::SendPsuReadModel>(dc, dp.slot_id);
    auto manufacturer = call_psu_command<ipmi::command::sdv::request::SendPsuReadManufacturer,
        ipmi::command::sdv::response::SendPsuReadManufacturer>(dc, dp.slot_id);
    auto revision = call_psu_command<ipmi::command::sdv::request::SendPsuReadRevision,
        ipmi::command::sdv::response::SendPsuReadRevision>(dc, dp.slot_id);
    auto serial = call_psu_command<ipmi::command::sdv::request::SendPsuReadSerial,
        ipmi::command::sdv::response::SendPsuReadSerial>(dc, dp.slot_id);
    auto power_out = call_psu_command<ipmi::command::sdv::request::SendPsuReadPowerOutput,
        ipmi::command::sdv::response::SendPsuReadPowerOutput>(dc, dp.slot_id);
    auto power_cap = call_psu_command<ipmi::command::sdv::request::SendPsuReadPowerCapacity,
        ipmi::command::sdv::response::SendPsuReadPowerCapacity>(dc, dp.slot_id);

    attribute::FruInfo fru{};

    if (agent_framework::model::utils::is_ascii_string(manufacturer.get_manufacturer())) {
        fru.set_manufacturer(manufacturer.get_manufacturer());
    }
    else {
        log_warning("rmm-discovery", "Psu manufacturer contains invalid characters: " << manufacturer.get_manufacturer());
    }

    if (agent_framework::model::utils::is_ascii_string(model.get_model())) {
        fru.set_model_number(model.get_model());
    }
    else {
        log_warning("rmm-discovery", "Psu model number contains invalid characters: " << model.get_model());
    }

    if (agent_framework::model::utils::is_ascii_string(serial.get_serial())) {
        fru.set_serial_number(serial.get_serial());
    }
    else {
        log_warning("rmm-discovery", "Psu serial number contains invalid characters: " << serial.get_serial());
    }

    res.set_fru_info(std::move(fru));

    if (agent_framework::model::utils::is_ascii_string(revision.get_revision())) {
        res.set_firmware_version(revision.get_revision());
    }
    else {
        log_warning("rmm-discovery", "Psu firmware version contains invalid characters: " << revision.get_revision());
    }
    res.set_power_capacity_watts(power_cap.get_power_capacity_value());
    res.set_last_power_output_watts(power_out.get_power_output_value());

    log_debug("rmm-discovery", "Psu data:");
    log_debug("rmm-discovery", "\tManufacturer: " << manufacturer.get_manufacturer());
    log_debug("rmm-discovery", "\tModel: " << model.get_model());
    log_debug("rmm-discovery", "\tSerial Number: " << revision.get_revision());
    log_debug("rmm-discovery", "\tPower capacity watts: " << power_cap.get_power_capacity_value());
    log_debug("rmm-discovery", "\tLast power output watts: " << power_out.get_power_output_value());

    return res;
}

template<>
ModelType<RmmType::Fan> discover<RmmType::Fan>(const std::string& parent, const DiscoveryContext&,
                                               const DiscoveryParams<RmmType::Fan>& dp) {
    log_debug("rmm-discovery", "Fan discovery");
    ModelType<RmmType::Fan> res(parent);
    res.set_status(attribute::Status(enums::State::Enabled, enums::Health::OK));
    res.set_slot_id(dp.slot_id);
    res.set_current_speed_units(enums::SpeedUnits::RPM);
    res.set_current_speed(dp.tachs);
    res.set_physical_context(enums::PhysicalContext::Exhaust);

    log_debug("rmm-discovery", "Fan: " << unsigned(dp.slot_id) << " speed: " << dp.tachs);
    return res;
}

template <>
ModelType<RmmType::Drawer> discover<RmmType::Drawer>(const std::string& parent, const DiscoveryContext& dc,
                                                     const DiscoveryParams<RmmType::Drawer>& dp) {
    log_debug("rmm-discovery", "Drawer discovery");
    ModelType<RmmType::Drawer> res{parent};
    enums::Health health = dp.is_alert_present ? enums::Health::Critical : enums::Health::OK;
    enums::State state = enums::State::Enabled;
    res.set_status(attribute::Status(state, health));
    res.set_type(enums::ChassisType::Drawer);
    res.set_power_zone(dp.pzone_uuid);
    res.set_thermal_zone(dp.tzone_uuid);
    res.set_parent_chassis(dp.zone_uuid);
    res.set_slot_id(dp.slot_id);
    /* TODO This has to be improved */
    res.set_location_id(std::to_string(int(dp.slot_id / 2 + dp.location_offset)));

    res.set_allowed_reset_actions({
        model::enums::ResetType::ForceRestart,
        model::enums::ResetType::GracefulRestart
    });

    agent::rmm::utils::FruReader fru_reader{};
    constexpr uint8_t FRU_DEVICE_ID = 0x00;
    fru_reader.read_and_parse(dc.ipmi, agent::rmm::loader::IpmiConfig::get_mbp_bridge(dp.slot_id), FRU_DEVICE_ID);

    attribute::FruInfo fru{};
    fru.set_serial_number(to_optional_string(fru_reader.get_serial_number()));
    fru.set_part_number(to_optional_string(fru_reader.get_part_number()));
    fru.set_manufacturer(to_optional_string(fru_reader.get_manufacturer()));
    fru.set_model_number(to_optional_string(fru_reader.get_model()));
    res.set_fru_info(fru);

    log_debug("rmm-discovery", "Drawer: " << unsigned(dp.slot_id) << " alert present: " << dp.is_alert_present);
    return res;
}


template<>
ModelType<RmmType::ThermalZone> discover<RmmType::ThermalZone>(const std::string& parent, const DiscoveryContext&,
                                                               const DiscoveryParams<RmmType::ThermalZone>&) {
    log_debug("rmm-discovery", "Thermal Zone discovery");
    ModelType<RmmType::ThermalZone> res{parent};
    res.set_status(attribute::Status(enums::State::Enabled, enums::Health::OK));
    res.add_collection(attribute::Collection{
        enums::CollectionName::Fans,
        enums::CollectionType::Fans
    });
    return res;
}


template<>
ModelType<RmmType::PowerZone> discover<RmmType::PowerZone>(const std::string& parent, const DiscoveryContext& dc,
                                                           const DiscoveryParams<RmmType::PowerZone>&) {
    log_debug("rmm-discovery", "Power Zone discovery");
    ModelType<RmmType::PowerZone> res{parent};
    res.add_collection(attribute::Collection{
        enums::CollectionName::Psus,
        enums::CollectionType::PSUs
    });

    ipmi::command::sdv::response::GetPsShelfCritical critical{};
    dc.ipmi->send(ipmi::command::sdv::request::GetPsShelfCritical{}, critical);
    ipmi::command::sdv::response::GetPzonePresence presence{};
    dc.ipmi->send(ipmi::command::sdv::request::GetPzonePresence{}, presence);
    enums::Health health = critical.is_critical() ? enums::Health::Critical : enums::Health::OK;
    enums::State state = presence.is_present() ? enums::State::Enabled : enums::State::Absent;
    res.set_status(attribute::Status(state, health));

    log_debug("rmm-discovery", "Power zone present/critical: " << presence.is_present() << "/" << critical.is_critical());

    return res;
}


template<>
ModelType<RmmType::Zone> discover<RmmType::Zone>(const std::string& parent, const DiscoveryContext&,
                                                 const DiscoveryParams<RmmType::Zone>& dp) {
    log_debug("rmm-discovery", "Zone discovery");
    ModelType<RmmType::Zone> res{parent};
    res.set_status(attribute::Status(enums::State::Enabled, enums::Health::OK));
    res.add_collection(attribute::Collection{
        enums::CollectionName::ThermalZones,
        enums::CollectionType::ThermalZones
        });
    res.add_collection(attribute::Collection{
        enums::CollectionName::PowerZones,
        enums::CollectionType::PowerZones});

    res.set_type(enums::ChassisType::Zone);
    res.set_parent_chassis(dp.rack_uuid);
    res.set_slot_id(dp.location_offset);

    attribute::FruInfo fru{};
    fru.set_manufacturer(dp.manufacturer_name);
    fru.set_model_number(dp.product_name);
    res.set_fru_info(fru);
    res.set_location_id(std::string{"Zone-"} + std::to_string(int(dp.location_offset)));

    log_debug("rmm-discovery", "Zone " << unsigned(dp.location_offset) << " data:");
    log_debug("rmm-discovery", "\tManufacturer: " << dp.manufacturer_name);
    log_debug("rmm-discovery", "\tModel number: " << dp.product_name);

    return res;
}


template<>
ModelType<RmmType::ZoneManager> discover<RmmType::ZoneManager>(const std::string& parent, const DiscoveryContext&,
                                                               const DiscoveryParams<RmmType::ZoneManager>& dp) {
    log_debug("rmm-discovery", "Zone Manager discovery");
    ModelType<RmmType::ZoneManager> res{parent};
    res.set_status(attribute::Status(enums::State::Enabled, enums::Health::OK));
    res.add_collection(attribute::Collection{
        enums::CollectionName::Chassis,
        enums::CollectionType::Chassis});

    res.add_collection(attribute::Collection{
        enums::CollectionName::NetworkInterfaces,
        enums::CollectionType::NetworkInterfaces});
    res.set_manager_type(enums::ManagerInfoType::ManagementController);
    res.set_firmware_version(dp.firmware_version);
    res.set_allowed_reset_actions({enums::ResetType::ForceRestart});

    log_debug("rmm-discovery", "Zone Manager data:");
    log_debug("rmm-discovery", "\tFirmware version " << dp.firmware_version);

    return res;
}


template<>
ModelType<RmmType::Rack> discover<RmmType::Rack>(const std::string& parent, const DiscoveryContext&,
                                                 const DiscoveryParams<RmmType::Rack>&) {
    log_debug("rmm-discovery", "Rack discovery");
    ModelType<RmmType::Rack> res{parent};
    res.set_disaggregated_power_cooling_support(false);
    res.set_status(attribute::Status(enums::State::Enabled, enums::Health::OK));
    res.set_type(enums::ChassisType::Rack);
    return res;
}


template<>
ModelType<RmmType::RackManager> discover<RmmType::RackManager>(const std::string& parent, const DiscoveryContext&,
                                                               const DiscoveryParams<RmmType::RackManager>&) {
    log_debug("rmm-discovery", "Rack Manager discovery");
    ModelType<RmmType::RackManager> res{parent};
    res.set_status(attribute::Status(enums::State::Enabled, enums::Health::OK));
    res.add_collection(attribute::Collection{
        enums::CollectionName::Chassis,
        enums::CollectionType::Chassis});

    res.add_collection(attribute::Collection{
        enums::CollectionName::NetworkInterfaces,
        enums::CollectionType::NetworkInterfaces});

    res.set_manager_type(enums::ManagerInfoType::RackManager);
    res.set_allowed_reset_actions({enums::ResetType::GracefulRestart});
    res.set_firmware_version(agent_framework::generic::Version::VERSION_STRING);
    return res;
}

template <>
ModelType<RmmType::RackInterface> discover<RmmType::RackInterface>(const std::string& parent, const DiscoveryContext&,
                                                                   const DiscoveryParams<RmmType::RackInterface>& dp) {
    log_debug("rmm-discovery", "Rack Interface discovery");
    ModelType<RmmType::RackInterface> res{parent};
    res.set_status(attribute::Status(enums::State::Enabled, enums::Health::OK));
    res.add_collection(attribute::Collection{
        enums::CollectionName::EthernetSwitchPortVlans,
        enums::CollectionType::EthernetSwitchPortVlans});

    res.set_name(dp.raw_data.name);
    res.set_frame_size(dp.raw_data.mtu);
    res.set_speed_mbps(dp.raw_data.speed);
    res.set_mac_address(dp.raw_data.mac_address);
    res.set_factory_mac_address(dp.raw_data.factory_mac_address);
    res.set_full_duplex(dp.raw_data.is_full_duplex);
    for (const auto& address : dp.raw_data.addresses) {
        model::attribute::Ipv4Address ip_address{};
        ip_address.set_address(address.ip);
        ip_address.set_subnet_mask(address.netmask);
        res.add_ipv4_address(std::move(ip_address));
    }
    return res;
}

template <>
ModelType<RmmType::RackVlan> discover<RmmType::RackVlan>(const std::string& parent, const DiscoveryContext&,
                                                         const DiscoveryParams<RmmType::RackVlan>& dp) {
    log_debug("rmm-discovery", "Rack Vlan discovery");
    ModelType<RmmType::RackVlan> res{parent};
    res.set_status(attribute::Status(enums::State::Enabled, enums::Health::OK));
    res.set_vlan_id(dp.vlan_id);
    log_debug("rmm-discovery", "Vlan id: " << unsigned(dp.vlan_id));
    return res;
}


template<>
ModelType<RmmType::ZoneInterface> discover<RmmType::ZoneInterface>(const std::string& parent, const DiscoveryContext& dc,
                                                                   const DiscoveryParams<RmmType::ZoneInterface>&) {
    log_debug("rmm-discovery", "Zone Interface discovery");
    ModelType<RmmType::ZoneInterface> res{parent};
    res.set_status(attribute::Status(enums::State::Enabled, enums::Health::OK));

    ipmi::command::generic::response::GetLanMacAddress mac_address{};
    dc.ipmi->send(ipmi::command::generic::request::GetLanMacAddress{}, mac_address);
    res.set_mac_address(mac_address.get_mac_address());
    res.set_factory_mac_address(mac_address.get_mac_address());

    ipmi::command::generic::response::GetLanIpAddress ip_address{};
    dc.ipmi->send(ipmi::command::generic::request::GetLanIpAddress{}, ip_address);
    ipmi::command::generic::response::GetLanNetMask netmask{};
    dc.ipmi->send(ipmi::command::generic::request::GetLanNetMask{}, netmask);
    ipmi::command::generic::response::GetLanIpAddressSource origin{};
    dc.ipmi->send(ipmi::command::generic::request::GetLanIpAddressSource{}, origin);
    ipmi::command::generic::response::GetLanGateway gateway{};
    dc.ipmi->send(ipmi::command::generic::request::GetLanGateway{}, gateway);
    attribute::Ipv4Address ip{};
    ip.set_address(ip_address.get_ip_address());
    ip.set_subnet_mask(netmask.get_netmask());
    ip.set_gateway(gateway.get_gateway());
    ip.set_address_origin(convert_ipmi_address_source_to_model_address_origin(origin.get_ip_address_source()));
    res.add_ipv4_address(ip);

    return res;
}

}
}
}
}
