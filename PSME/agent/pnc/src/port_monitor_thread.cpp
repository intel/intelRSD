/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 * @section DESCRIPTION
*/


#include "port_monitor_thread.hpp"
#include "state_machine/port_state_manager.hpp"
#include "gas/mrpc/link_status_retrieve.hpp"
#include "gas/mrpc/port_binding_info.hpp"
#include "gas/global_address_space_registers.hpp"
#include "agent-framework/module/pnc_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "discovery/discovery_manager.hpp"
#include "logger/logger_factory.hpp"

#include <chrono>



using namespace agent::pnc;
using namespace agent::pnc::state_machine;
using namespace agent::pnc::gas;
using namespace agent::pnc::gas::mrpc;
using namespace agent::pnc::nvme;
using namespace agent::pnc::discovery;
using namespace agent::pnc::discovery::builders;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::eventing;
using namespace agent::pnc::tools;

using StringVector = std::vector<std::string>;
using PsmVector = std::vector<std::unique_ptr<PortStateManager>>;

namespace {

bool get_is_link_up(const Port& port) {
    return port.get_status().get_state() == enums::State::Enabled;
}


bool get_is_being_erased(const Drive& drive) {
    return drive.get_is_being_erased();
}


std::tuple<bool, bool> get_is_bound(const GlobalAddressSpaceRegisters& gas, const PortBindingInfo& pbi,
                                    uint8_t port_id) {
    for (unsigned i = 0; i <= pbi.output.fields.info_count; ++i) {
        if (pbi.output.fields.port_binding_info[i].phy_port_id == port_id) {
            return std::make_tuple(
                pbi.output.fields.port_binding_info[i].partition_id != 0xFF,
                pbi.output.fields.port_binding_info[i].partition_id == gas.top.output.fields.current_partition_id);
        }
    }
    throw std::runtime_error("Port Id not in list while checking port binding info");

}


bool get_is_present(const uint64_t bitmask, const uint64_t phys_port_id) {
    // phys_port_id has values [0, 2, ..., 14] and [32, 34, ..., 46] that have to be converted into [0, 1, ..., 15]
    // divide by 2: [0, 2, ..., 14] and [32, 34, ... 46] are now [0, 1, ..., 7] and [16, 17, ... 23]
    uint8_t phys_port_bit = uint8_t(phys_port_id / 2u);
    // if greater than 7, remove 8: [0, 1, ..., 7] and [16, 17, ... 23] are transformed into [0, 1, ... 15]
    phys_port_bit = uint8_t(phys_port_bit > 7 ? phys_port_bit - 8 : phys_port_bit);
    return (bitmask & uint64_t(uint64_t(0x1) << phys_port_bit)) != 0x0;
}


std::tuple<StringVector, PsmVector> initialize_port_vectors(const DiscoveryManager& dm, const Toolset& t,
                                                            const std::string& switch_uuid,
                                                            const StringVector& port_uuids) {
    StringVector usps_uuid{};
    PsmVector psms{};
    for (const auto& port_uuid : port_uuids) {
        Port port = get_manager<Port>().get_entry(port_uuid);
        switch (port.get_port_type().value()) {
            // for USP port we will be updating its status
            case enums::PciePortType::UpstreamPort:
                log_debug("port-monitor", "PortMonitorThread: initializing upstream port "
                    << port.get_port_id());
                usps_uuid.push_back(port_uuid);
                break;
                // for DSP port we will be have to monitor its state to control discovery of devices connected to it
            case enums::PciePortType::DownstreamPort:
                log_debug("port-monitor", "PortMonitorThread: initializing downstream port "
                    << port.get_port_id());
                {   // braces required to keep this variable in the scope of single 'case'
                    std::shared_ptr<PortStateWorker> psw{new PortStateWorker(dm, t)};
                    std::unique_ptr<PortStateManager> psm{new PortStateManager(psw, port.get_port_id(), switch_uuid,
                                                                               port_uuid)};
                    psms.emplace_back(std::move(psm));
                }
                break;
                // we ignore Management ports
            case enums::PciePortType::ManagementPort:
                log_debug("port-monitor", "PortMonitorThread: ignoring management port "
                    << port.get_port_id());
                break;
            case enums::PciePortType::InterswitchPort:
            case enums::PciePortType::BidirectionalPort:
            case enums::PciePortType::UnconfiguredPort:
            default:
                log_debug("port-monitor", "PortMonitorThread: ignoring unsupported port type "
                    << port.get_port_id());
                log_warning("port-monitor", "Port of unsupported type found");
                break;
        }
    }
    return std::make_tuple(std::move(usps_uuid), std::move(psms));
}


uint64_t initialize_dsps(const PsmVector& psms, const DiscoveryManager& dm, const GlobalAddressSpaceRegisters& gas,
                         const Toolset& tools) {
    uint64_t presence_mask{0u};
    auto pbi = tools.gas_tool->get_all_port_binding_info(gas);
    for (auto& psm : psms) {
        auto port_uuid = psm->get_port_uuid();
        Port port = get_manager<Port>().get_entry(port_uuid);
        bool is_bound{false};
        bool is_bound_to_host{false};
        std::tie(is_bound, is_bound_to_host) = get_is_bound(gas, pbi, uint8_t(port.get_phys_port_id()));
        psm->init_binding(is_bound, is_bound_to_host);
        dm.update_port_status(gas, port_uuid);
        // model was updated - get new status
        bool is_link_up = get_is_link_up(get_manager<Port>().get_entry(port_uuid));
        psm->init_presence(is_link_up);
        if (is_link_up) {
            presence_mask |= (uint64_t(1u) << port.get_phys_port_id());
        }
    }
    return presence_mask;
}


void update_downstream_ports(const PsmVector& psms, const DiscoveryManager& dm, const GlobalAddressSpaceRegisters& gas,
                             const Toolset&, uint64_t presence_bitmask, const PortBindingInfo& pbi) {

    for (auto& psm : psms) {

        Port port = get_manager<Port>().get_entry(psm->get_port_uuid());
        log_debug("port-monitor", "PortMonitorThread: checking port " << port.get_port_id());

        // gather state variables
        bool is_device_present = get_is_present(presence_bitmask, port.get_phys_port_id());
        bool is_drive_present = (is_device_present ? psm->is_device_present() : false)
                                && get_manager<Drive>().entry_exists(psm->get_device_uuid());
        bool is_bound{false};
        bool is_bound_to_host{false};
        std::tie(is_bound, is_bound_to_host) = get_is_bound(gas, pbi, uint8_t(port.get_phys_port_id()));
        bool is_being_erased =
            (is_drive_present ? get_is_being_erased(get_manager<Drive>().get_entry(psm->get_device_uuid())) : false);

        // update status, state variables must not change
        dm.update_port_status(gas, psm->get_port_uuid());
        if (is_drive_present) {
            auto drive_ref = get_manager<Drive>().get_entry_reference(psm->get_device_uuid());
            // we only 'touch' drives that have no state overrides
            if (!(drive_ref->get_is_being_erased() || drive_ref->get_is_in_warning_state()
                  || drive_ref->get_is_in_critical_discovery_state() || drive_ref->get_is_being_discovered())) {

                if (is_bound) {
                    // drive is present and bound -> read smart
                    dm.update_drive_status(psm->get_port_uuid(), psm->get_device_uuid());
                }
                else {
                    // drive is present and unbound -> set StandbyOffline and keep previous smart health
                    auto status = drive_ref->get_status();
                    status.set_state(enums::State::StandbyOffline);
                    status.set_health(drive_ref->get_last_smart_health());
                    drive_ref->set_status(status);
                }
            }
        }

        // update state machine
        psm->update(is_device_present, is_bound, is_bound_to_host, is_being_erased);

        log_debug("port-monitor", "PortMonitorThread: finished checking port " << port.get_port_id());
    }
}


void
update_upstream_ports(const StringVector& uuids, const DiscoveryManager& dm, const GlobalAddressSpaceRegisters& gas,
                      const Toolset&) {
    for (const auto& uuid : uuids) {
        dm.update_port_status(gas, uuid);
    }
}


uint64_t get_presence(const Toolset& tools, const GlobalAddressSpaceRegisters& gas, uint64_t edk_mask) {
    auto chassis_uuid = tools.model_tool->get_chassis_uuid();
    auto chassis = get_manager<Chassis>().get_entry(chassis_uuid);
    if (enums::PlatformType::EDK == chassis.get_platform()) {
        return edk_mask;
    }
    else if (enums::PlatformType::MF3 == chassis.get_platform()) {
        return tools.gas_tool->get_presence_bitmask(gas);
    }
    else {
        throw std::runtime_error("Unknown platform type");
    }
}

}


PortMonitorThread::~PortMonitorThread() {
    m_is_running = false;
    m_condition.notify_one();
    if (m_thread.joinable()) {
        m_thread.join();
    }
}


void PortMonitorThread::task() {
    log_info("port-monitor", "PortMonitorThread: starting");

    std::vector<std::string> port_uuids = get_manager<Port>().get_keys(m_switch_uuid);
    log_info("port-monitor", "PortMonitorThread: found " << port_uuids.size() << " ports");

    // init gas
    Switch pcie_switch = get_manager<Switch>().get_entry(m_switch_uuid);
    GlobalAddressSpaceRegisters gas = GlobalAddressSpaceRegisters::get_default(pcie_switch.get_memory_path());
    gas.read_top();

    DiscoveryManager dm = DiscoveryManager::create(m_tools);

    // initialize managers for downstream ports and list all upstream ports
    StringVector usps;
    PsmVector dsps;

    std::tie(usps, dsps) = initialize_port_vectors(dm, m_tools, m_switch_uuid, port_uuids);

    uint64_t presence_bitmask{};
    PortBindingInfo pbi{gas.get_interface()};

    uint64_t edk_mask = initialize_dsps(dsps, dm, gas, m_tools);

    while (m_is_running) {
        std::unique_lock<std::mutex> lk(m_mutex);
        if (!m_is_discovery_finished
            || m_condition.wait_for(lk, std::chrono::seconds(m_interval)) == std::cv_status::timeout) {

            try {
                log_debug("port-monitor", "PortMonitorThread: monitor loop started!");
                // get data
                gas.read_top();
                presence_bitmask = get_presence(m_tools, gas, edk_mask);
                pbi = m_tools.gas_tool->get_all_port_binding_info(gas);
                log_debug("port-monitor", "PortMonitorThread: Presence bitmask: "
                    << std::hex << presence_bitmask << std::dec);
                // update port data
                update_downstream_ports(dsps, dm, gas, m_tools, presence_bitmask, pbi);
                update_upstream_ports(usps, dm, gas, m_tools);
                log_debug("port-monitor", "PortMonitorThread: monitor loop finished!");
            }
            catch (std::exception& e) {
                log_error("port-monitor", "PortMonitorThread: loop FAILED: " << e.what());
            }
            m_is_discovery_finished = true;
        }
    }

    log_debug("port-monitor", "PortMonitorThread stopped");
}


void PortMonitorThread::start() {
    m_is_running = true;
    m_thread = std::thread(&PortMonitorThread::task, this);
}
