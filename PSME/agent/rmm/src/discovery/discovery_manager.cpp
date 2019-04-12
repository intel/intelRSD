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
 * @file rmm/discovery/discovery_manager.cpp
 */

#include "ipmi/ipmi_controller.hpp"
#include "ipmi/command/sdv/rmm/get_chassis_presence.hpp"
#include "ipmi/command/sdv/rmm/get_chassis_alert.hpp"
#include "ipmi/command/sdv/rmm/get_fan_presence.hpp"
#include "ipmi/command/sdv/rmm/get_fan_tachs.hpp"
#include "ipmi/command/sdv/rmm/get_psu_presence.hpp"
#include "ipmi/command/sdv/rmm/set_id_field.hpp"
#include "ipmi/command/generic/get_device_id.hpp"

#include "discovery/discovery_manager.hpp"
#include "discovery/helpers/handle_resource.hpp"
#include "loader/ipmi_config.hpp"
#include "certificate_management/certificate_manager.hpp"

#include "configuration/configuration.hpp"

#include <functional>

using namespace agent::rmm::discovery;
using namespace agent::rmm::discovery::helpers;
using namespace agent_framework::model;
using namespace agent_framework::model::attribute;
using namespace agent_framework::module;

namespace {

// helper power/thermal zone functions

void remove_tzones_with_children(const DiscoveryContext& dc, const std::vector<std::string>& zones_uuids) {
    for (const auto& zone_uuid : zones_uuids) {
        auto children = get_manager<Fan>().get_keys(zone_uuid);
        for (const auto& child : children) {
            remove_metrics<RmmType::Fan>(dc, child);
            log_notify_and_remove<RmmType::Fan>(child, dc.event_collector);
        }
        remove_metrics<RmmType::ThermalZone>(dc, zone_uuid);
        log_notify_and_remove<RmmType::ThermalZone>(zone_uuid, dc.event_collector);
    }
}

void remove_pzones_with_children(const DiscoveryContext& dc, const std::vector<std::string>& zones_uuids) {
    for (const auto& zone_uuid : zones_uuids) {
        auto children = get_manager<Psu>().get_keys(zone_uuid);
        for (const auto& child : children) {
            log_notify_and_remove<RmmType::Psu>(child, dc.event_collector);
        }
        remove_metrics<RmmType::PowerZone>(dc, zone_uuid);
        log_notify_and_remove<RmmType::PowerZone>(zone_uuid, dc.event_collector);
    }
}

void update_pzone_with_children(const DiscoveryContext& dc, const std::string& zone_uuid,
        std::function<void(std::string)> child_discovery_func, const std::vector<uint8_t>& drawer_slots) {
    auto zone = get_manager<ModelType<RmmType::PowerZone>>().get_entry(zone_uuid);
    child_discovery_func(zone_uuid);

    // Psus have to be updated before zone, as zone uses their data
    DiscoveryParams<RmmType::PowerZone> dp{};
    for (const auto& psu_uuid : get_manager<Psu>().get_keys(zone_uuid)) {
        dp.psus.emplace_back(get_manager<Psu>().get_entry(psu_uuid));
        dp.drawer_slots = drawer_slots;
    }
    bool zone_changed =
        update<RmmType::PowerZone>(zone, dc, dp) || update_metrics<RmmType::PowerZone>(zone, dc, dp);
    log_notify_and_update<RmmType::PowerZone>(zone, dc.event_collector, zone_changed);
}

void update_tzone_with_children(const DiscoveryContext& dc,  const std::string& zone_uuid,
        std::function<void(std::string)> child_discovery_func, const std::vector<uint8_t>& drawer_slots) {
    auto zone = get_manager<ModelType<RmmType::ThermalZone>>().get_entry(zone_uuid);
    child_discovery_func(zone_uuid);

    DiscoveryParams<RmmType::ThermalZone> dp{};
    for (const auto& fan_uuid : get_manager<Fan>().get_keys(zone_uuid)) {
        dp.fan_slots.emplace_back(get_manager<Fan>().get_entry(fan_uuid).get_slot_id());
        dp.drawer_slots = drawer_slots;
    }
    bool zone_changed =
        update<RmmType::ThermalZone>(zone, dc, dp) || update_metrics<RmmType::ThermalZone>(zone, dc, dp);
    log_notify_and_update<RmmType::ThermalZone>(zone, dc.event_collector, zone_changed);
}

}

DiscoveryManager::~DiscoveryManager() {}

void DiscoveryManager::rack_discovery(const DiscoveryContext& dc) {
    auto racks_uuids = get_resources<RmmType::Rack>(DiscoveryParams<RmmType::Rack>{}, std::string{});
    if (0 == racks_uuids.size()) {
        add_rack(dc);
    }
    else if (1 == racks_uuids.size()) {
        update_rack(dc, racks_uuids.front());
    }
    else {
        throw std::runtime_error(std::string{"Too many racks in the model: "} + std::to_string(racks_uuids.size()));
    }
}

void DiscoveryManager::handle_interface_with_vlans(const DiscoveryContext& dc,
                                                     const net_reader::RawNetworkInterface& interface,
                                                     const std::string& manager_uuid) {
    log_debug("rmm-discovery", "Discovering Network Interface " << interface.name);
    DiscoveryParams<RmmType::RackInterface> interface_params{};
    interface_params.raw_data = interface;

    auto iface = discover<RmmType::RackInterface>(manager_uuid, dc, interface_params);
    auto iface_uuid = log_notify_and_add<RmmType::RackInterface>(iface, dc.event_collector);

    // discover vlans
    log_debug("rmm-discovery", "Checking vlans...");
    for (const auto& vlan_id : interface.vlans) {
        log_debug("rmm-discovery", "Discovering vlan...");
        DiscoveryParams<RmmType::RackVlan> vlan_params{};
        vlan_params.vlan_id = vlan_id;
        auto vlan = discover<RmmType::RackVlan>(iface_uuid, dc, vlan_params);
        log_notify_and_add<RmmType::RackVlan>(vlan, dc.event_collector);
    }
}

void DiscoveryManager::update_rack(const DiscoveryContext& dc, const std::string& rack_uuid) {
    log_debug("rmm-discovery", "Updating Rack...");
    auto rack = get_manager<Chassis>().get_entry(rack_uuid);
    bool rack_changed = update<RmmType::Rack>(rack, dc, DiscoveryParams<RmmType::Rack>{});
    log_notify_and_update<RmmType::Rack>(rack, dc.event_collector, rack_changed);

    log_debug("rmm-discovery", "Updating Rack Manager...");
    auto manager = get_manager<Manager>().get_entry(rack.get_parent_uuid());
    bool manager_changed = update<RmmType::RackManager>(manager, dc, DiscoveryParams<RmmType::RackManager>{});
    log_notify_and_update<RmmType::RackManager>(manager, dc.event_collector, manager_changed);

    /* If it is needed to update network interface or vlans - it should be added here */
    log_debug("rmm-discovery", "Updating interfaces...");
    for (const auto& interface : dc.net_reader->get_interfaces()) {
        DiscoveryParams<RmmType::RackInterface> interface_params{};
        interface_params.raw_data = interface;

        auto rack_interface_by_name = [&manager, &interface] (const NetworkInterface& ni) {
            return ni.get_parent_uuid() == manager.get_uuid() && ni.get_name() == interface.name;
        };

        auto interfaces = get_manager<NetworkInterface>().get_entries(rack_interface_by_name);
        if (interfaces.size() == 0) {
            // interface wasn't previously discovered
            handle_interface_with_vlans(dc, interface, manager.get_uuid());
        } else if (interfaces.size() == 1) {
            // update the interface
            bool interface_changed = update<RmmType::RackInterface>(interfaces.front(), dc, interface_params);
            log_notify_and_update<RmmType::RackInterface>(interfaces.front(), dc.event_collector, interface_changed);

            /* Update interface vlans */
            std::set<uint32_t> detected_vlans(interface.vlans.begin(), interface.vlans.end());

            std::set<uint32_t> all_known_vlans{};
            for (const auto& vlan : get_manager<EthernetSwitchPortVlan>().get_entries(
                interfaces.front().get_uuid())) {
                all_known_vlans.insert(vlan.get_vlan_id().value());
            }

            all_known_vlans.insert(detected_vlans.begin(), detected_vlans.end());

            for (const auto& vlan_id: all_known_vlans) {
                DiscoveryParams<RmmType::RackVlan> vlan_params{};
                vlan_params.vlan_id = vlan_id;
                bool is_present = detected_vlans.count(vlan_id) == 1;
                handle_resource<RmmType::RackVlan>(dc, vlan_params, interfaces.front().get_uuid(), is_present);
            }
        } else {
            throw std::runtime_error(std::string{"Too many rack network interfaces with name "}
                                     + interface.name + " in the model: " + std::to_string(interfaces.size()));
        }
    }
}

void DiscoveryManager::add_rack(const DiscoveryContext& dc) {
    /*
     * NOTE: this mechanism will NOT work if rack hotswap is to be supported as it would send events
     * in the wrong order. In the current implementation this method is always called before
     * registering to the REST server, therefore these events never confuse REST server.
     * RackInterface and RackVlan Add events are sent before sending notification about the RackManager.
     */

    // discover rack and rack manager
    log_debug("rmm-discovery", "Discovering Rack Manager...");
    auto manager = discover<RmmType::RackManager>("", dc, DiscoveryParams<RmmType::RackManager>{});
    log_debug("rmm-discovery", "Discovering Rack...");
    auto rack = discover<RmmType::Rack>(manager.get_uuid(), dc, DiscoveryParams<RmmType::Rack>{});
    manager.set_location(rack.get_uuid());
    log_notify_and_add<RmmType::RackManager>(manager, dc.event_collector, false);
    log_notify_and_add<RmmType::Rack>(rack, dc.event_collector, false);

    log_debug("rmm-discovery", "Checking interfaces...");
    // discover network interfaces
    for (const auto& interface : dc.net_reader->get_interfaces()) {
        handle_interface_with_vlans(dc, interface, manager.get_uuid());
    }

    log_debug("rmm-discovery", "Stabilizing Rack and Rack Manager...");
    // these resources depend on ethernet interfaces - so stablization was turned off before, we stabilize them now
    auto manager_uuid = stabilize<RmmType::RackManager>(manager.get_uuid());
    auto rack_uuid = stabilize<RmmType::Rack>(rack.get_uuid());
    manager = get_manager<Manager>().get_entry(manager_uuid);
    rack = get_manager<Chassis>().get_entry(rack_uuid);
    dc.event_collector->poll_add_event(model::enums::Component::Manager, manager_uuid, manager.get_parent_uuid());
    dc.event_collector->poll_add_event(model::enums::Component::Chassis, rack_uuid, rack.get_parent_uuid());

    m_rack_uuid = rack.get_uuid();
    m_rack_manager_uuid = manager.get_uuid();

    update_rack(dc, m_rack_uuid);
}

void DiscoveryManager::zone_discovery(const DiscoveryContext& dc, uint8_t location_offset) {
    DiscoveryParams<RmmType::Zone> dp{};
    dp.location_offset = location_offset;
    dp.rack_uuid = m_rack_uuid;
    dp.parent_id = get_manager<Chassis>().get_entry(m_rack_uuid).get_location_id();
    handle_zone(dc, dp, is_zone_present(dc, dp));
}

void DiscoveryManager::add_zone(const DiscoveryContext& dc, ZoneDiscoveryParams& dp) {

    // discover Zone and ZoneManager
    log_debug("rmm-discovery", "Discovering zone: ipmi calls...");
    ipmi::command::generic::response::GetDeviceId device_id{};
    dc.ipmi->send(ipmi::command::generic::request::GetDeviceId{}, device_id);
    dp.manufacturer_name = device_id.get_manufacturer_name();
    dp.product_name = device_id.get_product_name();
    DiscoveryParams<RmmType::ZoneManager> manager_dp{};
    manager_dp.firmware_version = device_id.get_firmware_version();

    log_debug("rmm-discovery", "Discovering zone manager...");
    auto manager = discover<RmmType::ZoneManager>("", dc, manager_dp);
    log_debug("rmm-discovery", "Discovering zone chassis...");
    auto zone = discover<RmmType::Zone>(manager.get_uuid(), dc, dp);
    log_debug("rmm-discovery", "Discovering zone interface...");
    auto interface = discover<RmmType::ZoneInterface>(manager.get_uuid(), dc,
        DiscoveryParams<RmmType::ZoneInterface>{});
    manager.set_location(zone.get_uuid());

    // These resources depend on themselves i.e. adds/stabilizations and eventing has to go in different order
    log_debug("rmm-discovery", "Adding Zone/ZoneManager/ZoneInterface...");
    log_notify_and_add<RmmType::ZoneManager>(manager, dc.event_collector, false);
    log_notify_and_add<RmmType::Zone>(zone, dc.event_collector, false);
    log_notify_and_add<RmmType::ZoneInterface>(interface, dc.event_collector, false);
    auto interface_uuid = stabilize<RmmType::ZoneInterface>(interface.get_uuid());
    auto manager_uuid = stabilize<RmmType::ZoneManager>(manager.get_uuid());
    auto zone_uuid = stabilize<RmmType::Zone>(zone.get_uuid());
    manager = get_manager<Manager>().get_entry(manager_uuid);
    zone = get_manager<Chassis>().get_entry(zone_uuid);
    interface = get_manager<NetworkInterface>().get_entry(interface_uuid);
    dc.event_collector->poll_add_event(model::enums::Component::Manager, manager_uuid, manager.get_parent_uuid());
    dc.event_collector->poll_add_event(model::enums::Component::Chassis, zone_uuid, zone.get_parent_uuid());
    dc.event_collector->poll_add_event(model::enums::Component::NetworkInterface, interface_uuid, interface.get_parent_uuid());

    // discover Thermal Zone and power zones
    log_debug("rmm-discovery", "Discovering thermal zone...");
    auto tzone = discover<RmmType::ThermalZone>(zone_uuid, dc, DiscoveryParams<RmmType::ThermalZone>{});
    log_debug("rmm-discovery", "Discovering power zone...");
    auto pzone = discover<RmmType::PowerZone>(zone_uuid, dc, DiscoveryParams<RmmType::PowerZone>{});
    log_debug("rmm-discovery", "Adding zones...");
    const auto stable_pzone_uuid = log_notify_and_add<RmmType::PowerZone>(pzone, dc.event_collector);
    const auto stable_tzone_uuid = log_notify_and_add<RmmType::ThermalZone>(tzone, dc.event_collector);
    build_metrics<RmmType::PowerZone>(stable_pzone_uuid, dc, {});
    build_metrics<RmmType::ThermalZone>(stable_tzone_uuid, dc, {});

    // finish reading missing fields during the update
    update_zone(dc, dp, zone_uuid);
}

void DiscoveryManager::handle_psus(const DiscoveryContext& dc, const std::string& pzone_uuid) {
    log_debug("rmm-discovery", "Handling psus...");
    response::GetPsuPresence presence{};
    try {
        dc.ipmi->send(request::GetPsuPresence{}, presence);
    }
    catch (const std::exception& e) {
        log_error("rmm-discovery", "Unable to read psus data: " << e.what());
        return;
    }

    for (uint8_t slot_id = 0; slot_id < presence.get_slot_count(); ++slot_id) {
        log_debug("rmm-discovery", "Psu presence on slot " << unsigned(slot_id) << " = " << presence.is_present(slot_id));
        DiscoveryParams<RmmType::Psu> dp{};
        dp.slot_id = slot_id;
        handle_resource<RmmType::Psu>(dc, dp, pzone_uuid, presence.is_present(slot_id));
    }
}

void DiscoveryManager::handle_fans(const DiscoveryContext& dc, const std::string& tzone_uuid) {
    log_debug("rmm-discovery", "Handling fans...");
    response::GetFanPresence presence{};
    response::GetFanTachs tachs{};
    try {
        dc.ipmi->send(request::GetFanPresence{}, presence);
        dc.ipmi->send(request::GetFanTachs{}, tachs);
    }
    catch (const std::exception& e) {
        log_error("rmm-discovery", "Unable to read fans data: " << e.what());
        return;
    }

    for (uint8_t slot_id = 0; slot_id < presence.get_slot_count(); ++slot_id) {
        log_debug("rmm-discovery", "Fan presence on slot " << unsigned(slot_id) << " = " << presence.is_present(slot_id));
        DiscoveryParams<RmmType::Fan> dp{};
        dp.slot_id = slot_id;
        dp.tachs = tachs.get_tach_reading(slot_id);
        handle_resource<RmmType::Fan>(dc, dp, tzone_uuid, presence.is_present(slot_id));
    }
}

void DiscoveryManager::handle_drawers(const DiscoveryContext& dc, DrawerDiscoveryParams& dp,
                                      std::vector<std::uint8_t>& drawer_slots) {
    log_debug("rmm-discovery", "Handling drawers...");
    response::GetChassisPresence presence{};
    response::GetChassisAlert alerts{};
    try {
      dc.ipmi->send(request::GetChassisPresence{}, presence);
      dc.ipmi->send(request::GetChassisAlert{}, alerts);
    }
    catch (const std::exception& e) {
        log_error("rmm-discovery", "Unable to read drawers data: " << e.what());
        return;
    }
    // update location_id
    bool rack_present = false;
    OptionalField<std::string> location_id{};
    auto racks_uuids = get_resources<RmmType::Rack>(DiscoveryParams<RmmType::Rack>{}, "");
    if (racks_uuids.size() > 0) {
        rack_present = true;
        location_id = get_manager<Chassis>().get_entry(racks_uuids.front()).get_location_id();
    }
    else {
        log_warning("rmm-discovery", "Rack not present!");
    }

    // send update drawers, send location ids and certificates
    for (uint8_t slot_id = 0; slot_id < presence.get_slot_count(); ++slot_id) {
        log_debug("rmm-discovery", "Drawer presence on slot " << unsigned(slot_id) << " = " << presence.is_present(slot_id));
        dp.slot_id = slot_id;
        dp.is_alert_present = alerts.is_alerted(slot_id);
        dp.parent_id = location_id;
        handle_resource<RmmType::Drawer>(dc, dp, m_rack_manager_uuid,
            presence.is_present(slot_id));
        if (presence.is_present(slot_id)) {
            try {
                if (rack_present) {
                    log_debug("rmm-discovery", "Sending location ids...");
                    send_location_id(dc, dp, location_id);
                }
                log_debug("rmm-discovery", "Updating certificates...");
                dc.certificate_manager->update_certificate(agent::rmm::CertificateManager::CertificateType::PODM, dc.ipmi,
                    agent::rmm::loader::IpmiConfig::get_psme_chassis_bridge(slot_id));
            }
            catch (const std::exception& e) {
                log_error("rmm-discovery", "Unable to update certificate on drawer on slot " << unsigned(slot_id)
                    << ": " << e.what());
            }

        }
    }

    /* Create vector of occupied slots for tzone and pzone sensor update */
    for (uint8_t slot_id = 0; slot_id < presence.get_slot_count(); ++slot_id) {
        if (presence.is_present(slot_id)) {
            drawer_slots.push_back(slot_id);
        }
    }
}

void DiscoveryManager::send_location_id(const DiscoveryContext& dc, const DrawerDiscoveryParams& dp,
        const OptionalField<std::string>& location_id) const {
    auto drawer_uuids = get_resources<RmmType::Drawer>(dp, std::string{});
    if (drawer_uuids.size() != 1) {
        throw std::runtime_error(std::string{"Invalid number of drawers found on slot, expected 1, found: "}
            + std::to_string(drawer_uuids.size()));
    }
    const auto& drawer = get_manager<Chassis>().get_entry(drawer_uuids.front());
    ipmi::IpmiController::Ptr controller = dc.ipmi;
    ipmi::BridgeInfo bridge = loader::IpmiConfig::get_psme_chassis_bridge(drawer.get_slot_id());

    ipmi::command::sdv::request::SetIdField rack_id_request{};
    ipmi::command::sdv::response::SetIdField rack_id_response{};
    rack_id_request.set_id_field_type(ipmi::command::sdv::IdFieldType::RACK_ID);
    rack_id_request.set_data_as_string(location_id);

    ipmi::command::sdv::request::SetIdField tray_id_request{};
    ipmi::command::sdv::response::SetIdField tray_id_response{};
    tray_id_request.set_id_field_type(ipmi::command::sdv::IdFieldType::TRAY_PUID);
    tray_id_request.set_data_as_number(std::stoi(drawer.get_location_id()));

    try {
        log_debug("rmm-discovery", "Updating RackId for drawer " << drawer.get_uuid()
            << " on slot " << unsigned(drawer.get_slot_id()));
        controller->send(rack_id_request, bridge, rack_id_response);
        log_debug("rmm-discovery", "Updating RackId for drawer finished successfully");
        log_debug("rmm-discovery", "Updating TrayId for drawer " << drawer.get_uuid()
            << " on slot " << unsigned(drawer.get_slot_id()));
        controller->send(tray_id_request, bridge, tray_id_response);
        log_debug("rmm-discovery", "Updating TrayId for drawer finished successfully");
    }
    catch (const ipmi::ResponseError& e) {
        log_error("rmm-discovery", "Error while executing setting ids for drawer " << drawer.get_uuid() << ": " << e.what()
            << " return code: " << static_cast<unsigned int>(e.get_completion_code()));
    }
    catch (const std::exception& e) {
        log_error("rmm-discovery", "Updating ids for drawer " << drawer.get_uuid() << " failed: " << e.what());
    }
}

void DiscoveryManager::handle_ptas(const DiscoveryContext& dc, const std::string&, const std::string&,
    const std::string& tzone_uuid) {

    log_debug("rmm-discovery", "Handling ptas...");

    dc.ptas->reset_data(0);
    /* Fill PTAS message with hardware data if needed
     * Legacy rmm code modelled each drawer as two trays:
     * Top tray consisted of top-left and top-right sleds and a 'half' of RRC sled between them middle
     * Bottom tray consisted of bottom-left and bottom-right sleds and the other half of RRC sled
     * Total RRC power (both halves) was assumed to be 70.
     */
    if (dc.ptas->send_receive_data()) {
        auto tzone = get_manager<ThermalZone>().get_entry(tzone_uuid);
        bool tzone_changed = update<RmmType::ThermalZone>(tzone, dc, DiscoveryParams<RmmType::ThermalZone>{});
        log_notify_and_update<RmmType::ThermalZone>(tzone, dc.event_collector, tzone_changed);
    }
}

void DiscoveryManager::update_zone(const DiscoveryContext& dc, ZoneDiscoveryParams& dp, const std::string& uuid) {
    // gather resources
    auto zone = get_manager<Chassis>().get_entry(uuid);
    auto manager = get_manager<Manager>().get_entry(zone.get_parent_uuid());
    auto interfaces = get_manager<NetworkInterface>().get_keys(zone.get_parent_uuid());
    auto pzones = get_manager<PowerZone>().get_keys(uuid);
    auto tzones = get_manager<ThermalZone>().get_keys(uuid);
    if (pzones.size() != 1 || tzones.size() != 1 || interfaces.size() != 1) {
        throw std::runtime_error("Invalid number of power zones/thermal zones/network interfaces under a zone!");
    }
    auto interface = get_manager<NetworkInterface>().get_entry(interfaces.front());

    // update zone and zone manager and network interface
    log_debug("rmm-discovery", "Updating zone chassis...");
    bool zone_changed = update<RmmType::Zone>(zone, dc, dp);
    log_debug("rmm-discovery", "Updating zone manager...");
    bool manager_changed = update<RmmType::ZoneManager>(manager, dc, DiscoveryParams<RmmType::ZoneManager>{});
    log_debug("rmm-discovery", "Updating zone interface...");
    bool interface_changed = update<RmmType::ZoneInterface>(interface, dc,
        DiscoveryParams<RmmType::ZoneInterface>{});
    log_notify_and_update<RmmType::Zone>(zone, dc.event_collector, zone_changed);
    log_notify_and_update<RmmType::ZoneManager>(manager, dc.event_collector, manager_changed);
    log_notify_and_update<RmmType::ZoneInterface>(interface, dc.event_collector, interface_changed);

    // discover drawers and inlet temperatures
    DrawerDiscoveryParams drawer_dp{};
    drawer_dp.location_offset = zone.get_slot_id();
    drawer_dp.zone_uuid = uuid;
    drawer_dp.pzone_uuid = pzones.front();
    drawer_dp.tzone_uuid = tzones.front();

    // handle drawers and fill vector of non-empty drawer slots for pzone/tzone updates
    std::vector<uint8_t> drawer_slots{};
    handle_drawers(dc, drawer_dp, drawer_slots);

    // update pzones/tzones and their children
    log_debug("rmm-discovery", "Updating power zone children...");
    update_pzone_with_children(dc, pzones.front(),
                               std::bind(std::mem_fn(&DiscoveryManager::handle_psus), *this, dc, std::placeholders::_1),
                               drawer_slots);
    log_debug("rmm-discovery", "Updating thermal zone children...");
    update_tzone_with_children(dc, tzones.front(),
                               std::bind(std::mem_fn(&DiscoveryManager::handle_fans), *this, dc, std::placeholders::_1),
                               drawer_slots);

    handle_ptas(dc, uuid, pzones.front(), tzones.front());
}

void DiscoveryManager::remove_zone(const DiscoveryContext& dc, ZoneDiscoveryParams& dp, const std::string& uuid) {
    // gather uuids
    auto zone = get_manager<Chassis>().get_entry(uuid);
    auto pzones = get_manager<PowerZone>().get_keys(uuid);
    auto tzones = get_manager<ThermalZone>().get_keys(uuid);
    auto interfaces = get_manager<NetworkInterface>().get_keys(zone.get_parent_uuid());
    // remove tzones and pzones
    remove_pzones_with_children(dc, pzones);
    remove_tzones_with_children(dc, tzones);
    // find and remove drawers
    auto drawers = get_manager<Chassis>().get_keys([&dp, &uuid](const Chassis& ch) { return ch.get_parent_chassis() == uuid; });
    for (const auto& drawer : drawers) {
        log_notify_and_remove<RmmType::Drawer>(drawer, dc.event_collector);
    }
    // remove rack, rack manager and network interface
    for (const auto& interface : interfaces) {
        log_notify_and_remove<RmmType::ZoneInterface>(interface, dc.event_collector);
    }
    log_notify_and_remove<RmmType::Zone>(zone.get_uuid(), dc.event_collector);
    log_notify_and_remove<RmmType::ZoneManager>(zone.get_parent_uuid(), dc.event_collector);
}

bool DiscoveryManager::is_zone_present(const DiscoveryContext& dc, ZoneDiscoveryParams&) {
    try {
        /* This is only to check if zone is present */
        ipmi::command::generic::response::GetDeviceId device_id{};
        dc.ipmi->send(ipmi::command::generic::request::GetDeviceId{}, device_id);
        return true;
    }
    catch (...) {
        /* Nothing - zone not present */
    }
    return false;
}

void DiscoveryManager::handle_zone(const DiscoveryContext& dc, ZoneDiscoveryParams& dp, bool is_present) {

    auto zones = get_resources<RmmType::Zone>(dp, std::string{});

    switch (get_action_type(zones.size() == 1, is_present)) {
    case DiscoveryAction::Add:
        try {
            add_zone(dc, dp);
        }
        catch (const std::exception& e) {
            log_error("rmm-discovery", "Unable to add zone: " << e.what());
        }
        break;
    case DiscoveryAction::Update:
        try {
            update_zone(dc, dp, zones.front());
        }
        catch (const std::exception& e) {
            log_error("rmm-discovery", "Unable to update zone " << zones.front() << ": " << e.what());
        }
        break;
    case DiscoveryAction::Remove:
        try {
            remove_zone(dc, dp, zones.front());
        }
        catch (const std::exception& e) {
            log_error("rmm-discovery", "Unable to remove zone " << zones.front() << ": " << e.what());
        }
        break;
    case DiscoveryAction::None:
    default:
        break;
    }
}
