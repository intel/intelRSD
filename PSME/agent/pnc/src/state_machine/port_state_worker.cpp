/*!
 * @copyright
 * Copyright (c) 2016-2018 Intel Corporation
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

GlobalAddressSpaceRegisters get_gas(const std::string& switch_uuid) {
    auto pcie_switch = get_manager<Switch>().get_entry(switch_uuid);
    GlobalAddressSpaceRegisters gas = GlobalAddressSpaceRegisters::get_default(pcie_switch.get_memory_path());
    gas.read_top();
    gas.read_partition(gas.top.output.fields.current_partition_id);
    return gas;
}

constexpr std::size_t SYSFS_UPDATE_TIME_SEC = 10;
constexpr std::size_t UNBIND_DISCOVERY_DELAY_SEC = 1;

}

PortStateWorker::~PortStateWorker() {}

uint8_t PortStateWorker::get_bridge_id(const std::string& switch_uuid, const std::string& port_uuid) const {

    log_debug("port-state-worker", "\tAction: gathering data...");
    Port port = get_manager<Port>().get_entry(port_uuid);
    auto gas = get_gas(switch_uuid);

    log_debug("port-state-worker", "\tAction: checking for bindings...");
    PortBindingInfo pbi = m_tools.gas_tool->get_port_binding_info(gas, uint8_t(port.get_phys_port_id()));
    uint8_t bridge_id = m_tools.gas_tool->get_logical_bridge_for_port(pbi);
    log_debug("port-state-worker", "\tAction: Bound to bridge " << unsigned(bridge_id));
    return bridge_id;
}

void PortStateWorker::ib_discovery(const std::string& switch_uuid, const std::string& port_uuid, uint8_t bridge_id,
    const std::string& drive_uuid) const {

    log_debug("port-state-worker", "\tAction: discovering (ib)...");
    if (!m_dm.ib_port_device_discovery(switch_uuid, port_uuid, bridge_id, drive_uuid)) {
        throw std::runtime_error{"In-band discovery procedure failed"};
    }
    log_debug("port-state-worker", "\tAction: In-band discovery successful");
}

uint8_t PortStateWorker::bind_to_host(const std::string& switch_uuid, const std::string& port_uuid) const {

    log_debug("port-state-worker", "\tAction: gathering data...");
    Port port = get_manager<Port>().get_entry(port_uuid);
    auto gas = get_gas(switch_uuid);

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
    auto gas = get_gas(switch_uuid);

    log_debug("port-state-worker", "\tAction: unbinding...");
    m_tools.gas_tool->unbind_management_host_driver(sw.get_bridge_path(), bridge_id);
    m_tools.gas_tool->unbind_from_partition(gas, gas.top.output.fields.current_partition_id, bridge_id);

    // wait for system to update sysfs
    std::this_thread::sleep_for(std::chrono::seconds(UNBIND_DISCOVERY_DELAY_SEC));
    log_debug("port-state-worker", "\tAction: unbinding successful");
}

void PortStateWorker::full_discovery(const std::string& switch_uuid, const std::string& port_uuid,
        uint8_t bridge_id) const {

    log_debug("port-state-worker", "\tAction: Action: Full (Oob + Ib) discovery...");
    try {
        std::string drive_uuid = oob_discovery(switch_uuid, port_uuid);
        ib_discovery(switch_uuid, port_uuid, bridge_id, drive_uuid);
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
    auto gas = get_gas(switch_uuid);

    // perform discovery
    log_debug("port-state-worker", "\tAction: discovering (oob)...");
    if (!m_dm.oob_port_device_discovery(gas, switch_uuid, port_uuid)) {
        throw std::runtime_error{"Out-of-band discovery procedure failed"};
    }

    // get function and drive uuids
    log_debug("port-state-worker", "\tAction: retrieving uuids...");
    std::string drive_uuid{};
    try {
        drive_uuid = get_drive_by_dsp_port(port_uuid);
    }
    catch (const std::invalid_argument&) {
        log_info("port-state-worker", "\tNon drive device found!");
    }
    log_debug("port-state-worker", "\tAction: oob discovery successful");

    return drive_uuid;
}

void PortStateWorker::remove(const std::string& switch_uuid, const std::string& port_uuid) const {
    log_debug("port-state-worker", "\tAction: gathering data...");
    auto gas = get_gas(switch_uuid);

    if (!m_dm.remove_devices_on_port(gas, port_uuid)) {
        throw std::runtime_error{"Remove procedure failed"};
    }

    log_debug("port-state-worker", "Action: removal completed");
}

std::string PortStateWorker::get_drive_by_dsp_port(const std::string& port_uuid) const {

    auto drives = m_tools.model_tool->get_drives_by_dsp_port_uuid(port_uuid);

    // we expect only one drive to be found
    if (1 != drives.size()) {
        log_debug("agent",
            "Invalid number of drives using specified dsp port, expected 1, found " << drives.size());
        throw std::invalid_argument("Invalid number of drives using specified dsp port");
    }

    return drives.front();
}

void PortStateWorker::lock_port(const std::string& port_uuid) const {

    // change drive status
    try {
        auto drive_uuid = this->get_drive_by_dsp_port(port_uuid);
        m_tools.model_tool->set_drive_is_being_discovered(drive_uuid, true);
        m_tools.model_tool->set_drive_status(drive_uuid,
                                             attribute::Status{enums::State::Starting, enums::Health::OK});
        m_tools.model_tool->send_event(m_tools.model_tool->get_chassis_uuid(), drive_uuid,
                enums::Component::Drive, Notification::Update);
    }
    catch (std::exception&) {
        log_debug("port-state-worker", "\tCannot change drive status");
    }

    // change endpoints status
    auto fabric_uuid = m_tools.model_tool->get_fabric_uuid();
    std::vector<std::string> endpoint_uuids = get_m2m_manager<Endpoint, Port>().get_parents(port_uuid);
    for (const auto& endpoint_uuid : endpoint_uuids) {
        get_manager<Endpoint>().get_entry_reference(endpoint_uuid)->set_status(
            attribute::Status{enums::State::Starting, enums::Health::OK}
        );
        m_tools.model_tool->send_event(fabric_uuid, endpoint_uuid, enums::Component::Endpoint, Notification::Update);
    }
}

void PortStateWorker::unlock_port(const std::string& port_uuid) const {

    // change endpoints status
    try {
        auto drive_uuid = this->get_drive_by_dsp_port(port_uuid);
        m_tools.model_tool->set_drive_is_being_discovered(drive_uuid, false);
    }
    catch (std::exception&) {
        log_debug("port-state-worker", "\tCannot change drive status");
    }

    // change endpoints status
    auto fabric_uuid = m_tools.model_tool->get_fabric_uuid();
    std::vector<std::string> endpoint_uuids = get_m2m_manager<Endpoint, Port>().get_parents(port_uuid);
    for (const auto& endpoint_uuid : endpoint_uuids) {
        get_manager<Endpoint>().get_entry_reference(endpoint_uuid)->set_status(
            attribute::Status{enums::State::Enabled, enums::Health::OK}
        );
        m_tools.model_tool->send_event(fabric_uuid, endpoint_uuid, enums::Component::Endpoint, Notification::Update);
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
    auto gas = get_gas(switch_uuid);
    m_tools.gas_tool->bind_endpoint_to_zone(gas, zone_uuid, endpoint_uuid);
    get_m2m_manager<Zone, Endpoint>().add_entry(zone_uuid, endpoint_uuid);
    m_tools.model_tool->send_event(m_tools.model_tool->get_fabric_uuid(), zone_uuid,
        enums::Component::Zone, Notification::Update);
}
