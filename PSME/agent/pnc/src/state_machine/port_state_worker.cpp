/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file state_machine/port_state_worker.cpp
 * @brief Implementation of the PortStateWorker
 * */

#include "agent-framework/eventing/utils.hpp"
#include "state_machine/port_state_worker.hpp"
#include "discovery/discovery_manager.hpp"
#include "gas/global_address_space_registers.hpp"

#include <thread>
#include <chrono>



using namespace agent::pnc::state_machine;
using namespace agent::pnc::discovery;
using namespace agent::pnc::gas;
using namespace agent::pnc::gas::mrpc;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework::eventing;
using namespace agent::pnc::tools;

namespace {

constexpr std::size_t SYSFS_UPDATE_TIME_SEC = 10;
constexpr std::size_t UNBIND_DISCOVERY_DELAY_SEC = 1;

}


PortStateWorker::~PortStateWorker() {}


uint8_t PortStateWorker::get_bridge_id(const std::string& switch_uuid, const std::string& port_uuid) const {

    log_debug("port-state-worker", "\tAction: gathering data...");
    Port port = get_manager<Port>().get_entry(port_uuid);
    auto gas = m_tools.gas_tool->get_gas(switch_uuid);

    log_debug("port-state-worker", "\tAction: checking for bindings...");
    PortBindingInfo pbi = m_tools.gas_tool->get_port_binding_info(gas, uint8_t(port.get_phys_port_id()));
    uint8_t bridge_id = m_tools.gas_tool->get_logical_bridge_for_port(pbi);
    log_debug("port-state-worker", "\tAction: Bound to bridge " << unsigned(bridge_id));
    return bridge_id;
}


void PortStateWorker::ib_discovery(const std::string& switch_uuid, const std::string& port_uuid, uint8_t bridge_id,
                                   const std::string& device_uuid) const {

    log_debug("port-state-worker", "\tAction: discovering (ib)...");
    if (!m_dm.ib_port_device_discovery(switch_uuid, port_uuid, bridge_id, device_uuid)) {
        throw std::runtime_error{"In-band discovery procedure failed"};
    }
    log_debug("port-state-worker", "\tAction: In-band discovery successful");
}


uint8_t PortStateWorker::bind_to_host(const std::string& switch_uuid, const std::string& port_uuid) const {

    log_debug("port-state-worker", "\tAction: gathering data...");
    Port port = get_manager<Port>().get_entry(port_uuid);
    auto gas = m_tools.gas_tool->get_gas(switch_uuid);

    log_debug("port-state-worker", "\tAction: binding to an empty bridge...");
    PartitionBindingInfo pbi = m_tools.gas_tool->get_partition_binding_info(
        gas, gas.top.output.fields.current_partition_id);
    uint8_t bridge_id = m_tools.gas_tool->get_available_bridge_id(pbi);
    m_tools.gas_tool->bind_to_partition(gas, uint8_t(port.get_phys_port_id()),
                                        gas.top.output.fields.current_partition_id, bridge_id);

    std::this_thread::sleep_for(std::chrono::seconds(::SYSFS_UPDATE_TIME_SEC));

    log_debug("port-state-worker", "\tAction: bound to bridge " << unsigned(bridge_id));
    log_debug("port-state-worker", "\tAction: binding successful");
    return bridge_id;
}


void PortStateWorker::unbind_from_host(const std::string& switch_uuid, uint8_t bridge_id) const {

    log_debug("port-state-worker", "\tAction: gathering data...");
    Switch sw = get_manager<Switch>().get_entry(switch_uuid);
    auto gas = m_tools.gas_tool->get_gas(switch_uuid);

    log_debug("port-state-worker", "\tAction: unbinding...");
    m_tools.gas_tool->unbind_management_host_driver(sw.get_bridge_path(), bridge_id);
    m_tools.gas_tool->unbind_from_partition(gas, gas.top.output.fields.current_partition_id, bridge_id);

    // wait for system to update sysfs
    std::this_thread::sleep_for(std::chrono::seconds(UNBIND_DISCOVERY_DELAY_SEC));
    log_debug("port-state-worker", "\tAction: unbinding from host successful");
}


void PortStateWorker::full_discovery(const std::string& switch_uuid, const std::string& port_uuid,
                                     uint8_t bridge_id) const {

    log_debug("port-state-worker", "\tAction: Action: Full (Oob + Ib) discovery...");
    try {
        std::string device_uuid = oob_discovery(switch_uuid, port_uuid);
        ib_discovery(switch_uuid, port_uuid, bridge_id, device_uuid);
    }
    catch (...) {
        log_debug("port-state-worker", "Full discovery procedure failed!");
        throw;
    }
    log_debug("port-state-worker", "\tAction: Full (Oob + Ib) discovery successful");
}


std::string PortStateWorker::oob_discovery(const std::string& switch_uuid, const std::string& port_uuid) const {

    log_debug("port-state-worker", "\tAction: gathering data...");
    Port port = get_manager<Port>().get_entry(port_uuid);
    auto gas = m_tools.gas_tool->get_gas(switch_uuid);

    // perform discovery
    log_debug("port-state-worker", "\tAction: discovering (oob)...");
    if (!m_dm.oob_port_device_discovery(gas, port_uuid)) {
        throw std::runtime_error{"Out-of-band discovery procedure failed"};
    }

    // get function and supported devices uuids
    log_debug("port-state-worker", "\tAction: retrieving uuids...");
    Uuid device_uuid{};
    try {
        device_uuid = get_device_uuid_by_dsp_port(port_uuid);
    }
    catch (const std::invalid_argument&) {
        log_info("port-state-worker", "\tUnsupported device found!");
    }
    log_debug("port-state-worker", "\tAction: oob discovery successful");

    return device_uuid;
}


void PortStateWorker::remove(const std::string& switch_uuid, const std::string& port_uuid) const {
    log_debug("port-state-worker", "\tAction: gathering data...");
    auto gas = m_tools.gas_tool->get_gas(switch_uuid);

    if (!m_dm.remove_devices_on_port(gas, port_uuid)) {
        throw std::runtime_error{"Remove procedure failed"};
    }

    log_debug("port-state-worker", "Action: removal completed");
}


Uuid PortStateWorker::get_device_uuid_by_dsp_port(const Uuid& port_uuid) const {

    auto devices = m_tools.model_tool->get_devices_by_dsp_port_uuid(port_uuid);

    // we expect only one device to be found
    if (1 != devices.size()) {
        log_debug("agent",
                  "Invalid number of devices using specified dsp port, expected 1, found " << devices.size());
        throw std::invalid_argument("Invalid number of devices using specified dsp port");
    }

    return devices.front();
}


void PortStateWorker::lock_port(const Uuid& port_uuid) const {

    // change device status
    try {
        auto device_uuid = this->get_device_uuid_by_dsp_port(port_uuid);
        m_tools.model_tool->set_device_is_being_discovered(device_uuid, true);
        if (get_manager<Drive>().entry_exists(device_uuid)) {
            //TODO: Add set_processor_status() or set_status<Processor>() after implementing telemetry
            try {
                m_tools.model_tool->set_drive_status(device_uuid,
                                                     attribute::Status{enums::State::Starting, enums::Health::OK});

                send_event(device_uuid, enums::Component::Drive,
                           enums::Notification::Update, m_tools.model_tool->get_chassis_uuid());
            }
            catch (std::exception&) {
                log_debug("port-state-worker", "\tCannot change drive status");
            }
        }
    }
    catch (std::exception&) {
        log_debug("port-state-worker", "\tCannot change device status");
    }

    // change endpoints status
    auto fabric_uuid = m_tools.model_tool->get_fabric_uuid();
    std::vector<std::string> endpoint_uuids = get_m2m_manager<Endpoint, Port>().get_parents(port_uuid);
    for (const auto& endpoint_uuid : endpoint_uuids) {
        get_manager<Endpoint>().get_entry_reference(endpoint_uuid)->set_status(
            attribute::Status{enums::State::Starting, enums::Health::OK}
        );
        send_event(endpoint_uuid, enums::Component::Endpoint, enums::Notification::Update, fabric_uuid);
    }
}


void PortStateWorker::unlock_port(const Uuid& port_uuid) const {

    // change endpoints status
    try {
        auto device_uuid = this->get_device_uuid_by_dsp_port(port_uuid);
        m_tools.model_tool->set_device_is_being_discovered(device_uuid, false);
    }
    catch (std::exception&) {
        log_debug("port-state-worker", "\tCannot change device status");
    }

    // change endpoints status
    auto fabric_uuid = m_tools.model_tool->get_fabric_uuid();
    std::vector<std::string> endpoint_uuids = get_m2m_manager<Endpoint, Port>().get_parents(port_uuid);
    for (const auto& endpoint_uuid : endpoint_uuids) {
        get_manager<Endpoint>().get_entry_reference(endpoint_uuid)->set_status(
            attribute::Status{enums::State::Enabled, enums::Health::OK}
        );
        send_event(endpoint_uuid, enums::Component::Endpoint, enums::Notification::Update, fabric_uuid);
    }
}


PortStateWorker::ZoneEndpointVector PortStateWorker::get_bindings_on_port(const std::string& port_uuid) const {
    auto endpoints = get_enabled_endpoints_on_port(port_uuid);
    ZoneEndpointVector bindings{};
    for (const auto& endpoint_uuid : endpoints) {
        auto zones = get_m2m_manager<Zone, Endpoint>().get_parents(endpoint_uuid);
        std::sort(zones.begin(), zones.end());
        auto last = std::unique(zones.begin(), zones.end());
        zones.erase(last, zones.end());
        for (const auto& zone_uuid : zones) {
            bindings.emplace_back(std::make_tuple(zone_uuid, endpoint_uuid));
        }
    }
    return bindings;
}


std::vector<std::string> PortStateWorker::get_enabled_endpoints_on_port(const std::string& port_uuid) const {
    auto endpoints = get_m2m_manager<Endpoint, Port>().get_parents(port_uuid);
    std::vector<std::string> result{};
    for (const auto& endpoint_uuid : endpoints) {
        auto endpoint = get_manager<Endpoint>().get_entry(endpoint_uuid);
        if (endpoint.get_status().get_state() == enums::State::Enabled) {
            result.push_back(endpoint_uuid);
        }
    }
    return result;
}


void PortStateWorker::attach_endpoint_to_zone(const std::string& switch_uuid, const std::string& endpoint_uuid,
                                              const std::string& zone_uuid) const {
    log_debug("port-state-worker", "\tAction: binding endpoint: " << endpoint_uuid << " to the zone: "
                                                                  << zone_uuid);
    auto gas = m_tools.gas_tool->get_gas(switch_uuid);
    m_tools.gas_tool->bind_endpoint_to_zone(gas, zone_uuid, endpoint_uuid);
    get_m2m_manager<Zone, Endpoint>().add_entry(zone_uuid, endpoint_uuid);
    send_event(zone_uuid, enums::Component::Zone, enums::Notification::Update, m_tools.model_tool->get_fabric_uuid());
}


void PortStateWorker::unbind_port(const Uuid& switch_uuid, const Uuid& port_uuid) {
    auto gas = m_tools.gas_tool->get_gas(switch_uuid);
    auto port = get_manager<Port>().get_entry(port_uuid);
    auto port_info = m_tools.gas_tool->get_port_binding_info(gas, static_cast<uint8_t>(port.get_phys_port_id()));
    m_tools.gas_tool->unbind_from_partition(gas, port_info.output.fields.port_binding_info->partition_id,
                                            port_info.output.fields.port_binding_info->logical_bridge_id);
    log_debug("port-state-worker", "\tAction: unbinding successful");
}


bool PortStateWorker::is_port_in_valid_zone(const Uuid& port_uuid) {
    auto bindings = get_bindings_on_port(port_uuid);
    if (!bindings.empty()) {

        auto not_equal_zone_in_binding = [](const ZoneEndpointPair& p1, const ZoneEndpointPair& p2) -> bool {
            return std::get<0>(p1) != std::get<1>(p2);
        };

        bool bindings_in_the_same_zone =
            std::adjacent_find(bindings.begin(), bindings.end(), not_equal_zone_in_binding) == bindings.end();

        if (!bindings_in_the_same_zone) {
            throw std::runtime_error{"Port exists in several zones by different endpoints"};
        }

        if (Zone::is_initiator_in_zone(std::get<0>(bindings[0]))) {
            return true;
        }
    }
    return false;
}
