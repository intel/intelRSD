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
 * @file rmm/discovery/helpers/update.hpp
 */

#pragma once

#include "discovery/helpers/common.hpp"
#include "discovery/helpers/get_resources.hpp"

#include "ipmi/command/sdv/rmm/get_pzone_presence.hpp"
#include "ipmi/command/sdv/rmm/get_psshelf_critical.hpp"
#include "ipmi/command/sdv/rmm/send_psu_command.hpp"
#include "ipmi/command/sdv/rmm/get_ps_input_power.hpp"
#include "ipmi/command/sdv/rmm/get_ps_dc_current.hpp"
#include "ipmi/command/sdv/rmm/get_pwm.hpp"
#include "agent-framework/module/constants/chassis.hpp"
#include "database/persistent_attributes.hpp"

namespace agent {
namespace rmm {
namespace discovery {
namespace helpers {

namespace request = ipmi::command::sdv::request;
namespace response = ipmi::command::sdv::response;

/*!
 * @brief This is a generic update function for a resource. It uses one parameter - type of the resource to be
 * discovered (TYPE). It accepts three arguments: reference to the resource to be updated, discovery context
 * that is meant to provide access to ipmi controller and additional discovery parameters stored in discovery
 * params struct. It returns true if anything was changed and update/events are required. Default implementation
 * does nothing and returns false (no update needed).
 * */
template <RmmType TYPE>
bool update(ModelType<TYPE>&, const DiscoveryContext&, const DiscoveryParams<TYPE>&) {
    log_debug("rmm-discovery", "Generic rmm update for a base type: " << get_model_component<TYPE>().to_string());
    return false;
}

template <>
bool update<RmmType::Psu>(ModelType<RmmType::Psu>& psu, const DiscoveryContext& dc,
                          const DiscoveryParams<RmmType::Psu>& dp) {
    log_debug("rmm-discovery", "Psu " << static_cast<unsigned>(psu.get_slot_id()) << " update");
    bool changed = false;

    auto operation_state = call_psu_command<ipmi::command::sdv::request::SendPsuReadOperation,
        ipmi::command::sdv::response::SendPsuReadOperation>(dc, dp.slot_id);
    log_debug("rmm-discovery", "Operation: 0x" << std::hex << unsigned(operation_state.get_operation_byte()));

    enums::State state = operation_state.is_enabled() ? enums::State::Enabled : enums::State::Disabled;


    enums::Health health = enums::Health::OK;
    auto psu_sta = call_psu_command<ipmi::command::sdv::request::SendPsuReadMfrPsuSta,
        ipmi::command::sdv::response::SendPsuReadMfrPsuSta>(dc, dp.slot_id);
    log_debug("rmm-discovery", "ALM STA: 0x" << std::hex << psu_sta.get_alm_sta()
                            << ", AC " << (psu_sta.ac_has_failed() ? "failed" : "OK"));

    if (psu_sta.errors_present()) {
        health = enums::Health::Critical;
    }
    else if (psu_sta.ac_has_failed() || psu_sta.warnings_present()) {
        health = enums::Health::Warning;
    }


    auto status = psu.get_status();
    if (status.get_health() != health || status.get_state() != state) {
        log_info("rmm-discovery", "Psu (" << unsigned(psu.get_slot_id()) << ") status changed: state: "
            << status.get_state().to_string() << " -> " << state.to_string() << ", health: "
            << status.get_health() << " -> " << health.to_string());

        status.set_state(state);
        status.set_health(health);
        psu.set_status(status);
        changed = true;
    }

    auto power_out = call_psu_command<ipmi::command::sdv::request::SendPsuReadPowerOutput,
        ipmi::command::sdv::response::SendPsuReadPowerOutput>(dc, dp.slot_id);
    auto power_out_value = power_out.get_power_output_value();

    if (!psu.get_last_power_output_watts().has_value() || power_out_value != psu.get_last_power_output_watts()) {
        log_debug("rmm-discovery", "Psu (" << unsigned(psu.get_slot_id()) << ") last output power changed: "
            << psu.get_last_power_output_watts() << " -> " << power_out.get_power_output_value());
        psu.set_last_power_output_watts(power_out_value);
        changed = true;
    }
    return changed;
}

template <>
bool update<RmmType::Drawer>(ModelType<RmmType::Drawer>& chassis, const DiscoveryContext&,
                             const DiscoveryParams<RmmType::Drawer>& dp) {
    log_debug("rmm-discovery", "Drawer update");
    bool changed = false;
    auto status = chassis.get_status();
    enums::Health current_health = dp.is_alert_present ? enums::Health::Critical : enums::Health::OK;
    if (status.get_health() != current_health) {
        log_debug("rmm-discovery", "Drawer (" << unsigned(chassis.get_slot_id()) << ") health changed: "
                                              << status.get_health() << " -> " << current_health);
        status.set_health(current_health);
        chassis.set_status(status);
        changed = true;
    }

    std::string val{};
    OptionalField<std::string> asset_tag{};
    if (database::PersistentAttributes::get_instance().get_value(chassis.get_uuid(),
        agent_framework::model::literals::Chassis::ASSET_TAG, val)) {

        asset_tag = val;
    }
    if (asset_tag != chassis.get_asset_tag()) {
        log_debug("rmm-discovery", "Drawer (" << unsigned(chassis.get_slot_id()) << ") "
                                              << "asset_tag " << chassis.get_asset_tag() << " -> " << asset_tag);
        chassis.set_asset_tag(asset_tag);
        changed = true;
    }

    if (chassis.get_parent_id() != dp.parent_id) {
        log_debug("rmm-discovery", "Drawer (" << unsigned(chassis.get_slot_id()) << ") "
                                              << "parent id " << chassis.get_parent_id() << " -> " << asset_tag);
        chassis.set_parent_id(dp.parent_id);
        changed = true;
    }

    return changed;
}

template<>
bool update<RmmType::Zone>(ModelType<RmmType::Zone>& chassis, const DiscoveryContext&,
                           const DiscoveryParams<RmmType::Zone>& dp) {
    log_debug("rmm-discovery", "Zone update");
    bool changed = false;
    std::string val{};

    OptionalField<std::string> asset_tag{};
    if (database::PersistentAttributes::get_instance().get_value(chassis.get_uuid(),
        agent_framework::model::literals::Chassis::ASSET_TAG, val)) {

        asset_tag = val;
    }
    if (asset_tag != chassis.get_asset_tag()) {
        log_debug("rmm-discovery", "Zone (" << unsigned(chassis.get_slot_id()) << ") "
                                              << "asset_tag " << chassis.get_asset_tag() << " -> " << asset_tag);
        chassis.set_asset_tag(asset_tag);
        changed = true;
    }
    if (chassis.get_parent_id() != dp.parent_id) {
        log_debug("rmm-discovery", "Zone (" << unsigned(chassis.get_slot_id()) << ") "
                                              << "parent id " << chassis.get_parent_id() << " -> " << asset_tag);
        chassis.set_parent_id(dp.parent_id);
        changed = true;
    }

    return changed;
}

template <>
bool update<RmmType::Rack>(ModelType<RmmType::Rack>& chassis, const DiscoveryContext&,
                           const DiscoveryParams<RmmType::Rack>&) {
    log_debug("rmm-discovery", "Rack update");
    bool changed = false;

    std::string val{};
    OptionalField<std::string> asset_tag{};
    if (database::PersistentAttributes::get_instance().get_value(chassis.get_uuid(),
        agent_framework::model::literals::Chassis::ASSET_TAG, val)) {

        asset_tag = val;
    }
    if (asset_tag != chassis.get_asset_tag()) {
        log_debug("rmm-discovery", "Rack asset tag " << chassis.get_asset_tag() << " -> " << asset_tag);
        chassis.set_asset_tag(asset_tag);
        changed = true;
    }

    OptionalField<std::string> geo_tag{};
    if (database::PersistentAttributes::get_instance().get_value(chassis.get_uuid(),
        agent_framework::model::literals::Chassis::GEO_TAG, val)) {

        geo_tag = val;
    }
    if (geo_tag != chassis.get_geo_tag()) {
        log_debug("rmm-discovery", "Rack geo tag " << chassis.get_geo_tag() << " -> " << geo_tag);
        chassis.set_geo_tag(geo_tag);
        changed = true;
    }

    OptionalField<std::string> location_id{};
    if (database::PersistentAttributes::get_instance().get_value(chassis.get_uuid(),
        agent_framework::model::literals::Chassis::LOCATION_ID, val)) {

        location_id = val;
    }
    if (!location_id.has_value()) {
        /* This is a default value as this field cannot be null */
        location_id = "1";
    }
    if (location_id != chassis.get_location_id()) {
        log_debug("rmm-discovery", "Rack location id " << chassis.get_location_id() << " -> " << location_id);
        chassis.set_location_id(location_id);
        changed = true;
    }

    return changed;
}

template <>
bool update<RmmType::PowerZone>(ModelType<RmmType::PowerZone>& pz, const DiscoveryContext& dc,
                                const DiscoveryParams<RmmType::PowerZone>& dp) {
    log_debug("rmm-discovery", "Power Zone update");
    bool changed = false;

    response::GetPsShelfCritical critical{};
    dc.ipmi->send(request::GetPsShelfCritical{}, critical);
    response::GetPzonePresence presence{};
    dc.ipmi->send(request::GetPzonePresence{}, presence);
    enums::Health health = critical.is_critical() ? enums::Health::Critical : enums::Health::OK;
    enums::State state = presence.is_present() ? enums::State::Enabled : enums::State::Absent;
    auto status = pz.get_status();
    if (status.get_health() != health || status.get_state() != state) {
        log_debug("rmm-discovery", "Power zone state changed: " << status.get_state() << " -> " << state);
        log_debug("rmm-discovery", "Power zone health changed: " << status.get_health() << " -> " << health);
        status.set_health(health);
        status.set_state(state);
        pz.set_status(status);
        changed = true;
    }

    OptionalField<int> power_capacity;
    OptionalField<int> power_allocated;
    for (const auto& psu : dp.psus) {
        if (psu.get_power_capacity_watts().has_value()) {
            power_capacity = power_capacity + int(psu.get_power_capacity_watts().value());
        }
        if (psu.get_last_power_output_watts().has_value()) {
            power_allocated = power_allocated + int(psu.get_last_power_output_watts().value());
        }
    }
    if (pz.get_power_allocated_watts() != power_allocated || pz.get_power_capacity_watts() != power_capacity) {
        if (power_capacity.has_value() && power_allocated.has_value()) {
            pz.set_power_available_watts(power_capacity - power_allocated);
        }
        else {
            pz.set_power_available_watts({});
        }
    }
    if (pz.get_power_allocated_watts() != power_allocated) {
        log_debug("rmm-discovery", "Power zone allocated watts changed: "
            << pz.get_power_allocated_watts() << " -> " << power_allocated);
        pz.set_power_allocated_watts(power_allocated);
        changed = true;
    }
    if (pz.get_power_capacity_watts() != power_capacity) {
        log_debug("rmm-discovery", "Power zone capacity watts changed: "
            << pz.get_power_capacity_watts() << " -> " << power_capacity);
        pz.set_power_capacity_watts(power_capacity);
        changed = true;
    }

    return changed;
}

template <>
bool update<RmmType::ThermalZone>(ModelType<RmmType::ThermalZone>& tz, const DiscoveryContext& dc,
                                  const DiscoveryParams<RmmType::ThermalZone>& dp) {
    log_debug("rmm-discovery", "Thermal zone update");
    bool changed = false;

    if (dc.ptas->is_data_valid()) {
        auto volumetric_airflow = tz.get_volumetric_airflow_cfm();
        auto new_volumetric_airflow = int(dc.ptas->get_volumetric_airflow());
        if (!volumetric_airflow.has_value() || volumetric_airflow != new_volumetric_airflow) {
            tz.set_volumetric_airflow_cfm(new_volumetric_airflow);
            log_debug("rmm-discovery", "Thermal zone volumetric airflow changed: "
                << volumetric_airflow << " -> " << new_volumetric_airflow);
            changed = true;
        }
    }

    OptionalField<int32_t> new_pwm{};
    for (const auto& fan_slot : dp.fan_slots) {
        try {
            request::GetPwm pwm_request{};
            response::GetPwm pwm_response{};
            pwm_request.set_asset_index(fan_slot);
            dc.ipmi->send(pwm_request, pwm_response);
            new_pwm = pwm_response.get_pwm_value();
            /* If value was read - we do not have to check other fans */
            break;
        }
        catch (const std::exception& e) {
            log_warning("rmm-discovery", "Unable to read fan pwm on slot " << unsigned(fan_slot)
                                      << ": " << e.what());
        }
    }
    if (tz.get_desired_speed_pwm() != new_pwm) {
        log_debug("rmm-discovery", "Thermal zone desired speed PWM changed: "
            << tz.get_desired_speed_pwm() << " -> " << new_pwm);
        tz.set_desired_speed_pwm(new_pwm);
        changed = true;
    }

    return changed;
}

template <>
bool update<RmmType::RackInterface>(ModelType<RmmType::RackInterface>& interface, const DiscoveryContext&,
                                  const DiscoveryParams<RmmType::RackInterface>& dp) {
    using attribute::Ipv4Address;

    log_debug("rmm-discovery", "Rack interface update");
    bool changed = false;

    auto& raw = dp.raw_data;

    if (interface.get_frame_size() != raw.mtu) {
        log_debug("rmm-discovery", "Interface frame size changed: " << interface.get_frame_size() << " -> " << raw.mtu);
        interface.set_frame_size(raw.mtu);
        changed = true;
    }

    if (interface.get_speed_mbps() != raw.speed) {
        log_debug("rmm-discovery", "Interface speed changed: " << interface.get_speed_mbps() << " -> " << raw.speed);
        interface.set_speed_mbps(raw.speed);
        changed = true;
    }

    if (interface.get_mac_address() != raw.mac_address) {
        log_debug("rmm-discovery", "Interface MAC address changed: " << interface.get_mac_address() << " -> " << raw.mac_address);
        interface.set_mac_address(raw.mac_address);
        changed = true;
    }

    if (interface.get_factory_mac_address() != raw.factory_mac_address) {
        log_debug("rmm-discovery", "Interface factory MAC address changed: " << interface.get_factory_mac_address()
                                                                             << " -> " << raw.factory_mac_address);
        interface.set_factory_mac_address(raw.factory_mac_address);
        changed = true;
    }

    if (interface.get_full_duplex() != raw.is_full_duplex) {
        log_debug("rmm-discovery", "Interface full duplex changed: " << interface.get_full_duplex()
                                                                             << " -> " << raw.is_full_duplex);
        interface.set_full_duplex(raw.is_full_duplex);
        changed = true;
    }


    bool ip_changed = false;
    auto old_addresses = interface.get_ipv4_addresses();

    attribute::Array<attribute::Ipv4Address> new_addresses{};
    for (const auto& address : dp.raw_data.addresses) {
        model::attribute::Ipv4Address ip_address{};
        ip_address.set_address(address.ip);
        ip_address.set_subnet_mask(address.netmask);
        new_addresses.add_entry(std::move(ip_address));
    }

    if (old_addresses.size() != new_addresses.size()) {
        ip_changed = true;
    } else {
        auto address_comparator = [](const Ipv4Address& left, const Ipv4Address& right) {
            return left.get_address() <= right.get_address();
        };

        std::sort(old_addresses.begin(), old_addresses.end(), address_comparator);
        std::sort(new_addresses.begin(), new_addresses.end(), address_comparator);

        for (size_t i = 0; i < old_addresses.size(); ++i) {
            if (old_addresses[i].get_address() != new_addresses[i].get_address()) {
                ip_changed = true;
                break;
            }
        }
    }

    if (ip_changed) {
        log_debug("rmm-discovery", "Interface IP addresses changed");
        interface.set_ipv4_addresses(new_addresses);
        changed = true;
    }

    return changed;
}


}
}
}
}
