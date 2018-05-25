/*!
 * @brief Implementation of Bmc class
 *
 * @copyright
 * Copyright (c) 2017-2018 Intel Corporation
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
 * @file bmc.cpp
 */

#include "status/bmc.hpp"
#include "status/system_power_state_thread.hpp"
#include "status/system_boot_options_thread.hpp"
#include "telemetry/compute_telemetry_service.hpp"
#include "telemetry/telemetry_runner.hpp"
#include "discovery/discovery_manager.hpp"
#include "tree_stability/compute_tree_stabilizer.hpp"
#include "agent-framework/module/compute_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/chassis_components.hpp"
#include "agent-framework/eventing/events_queue.hpp"
#include "ipmi/command/generic/get_device_id.hpp"
#include "ipmi/utils/sdv/mdr_region_accessor.hpp"

using namespace agent::compute;
using namespace agent_framework::module;

using agent_framework::model::enums::Component;
using agent_framework::eventing::Notification;
using agent::compute::Bmc;

namespace {

OptionalField<std::uint16_t> read_platform_id(Bmc& bmc) {
    try {
        log_debug("bmc", bmc.get_id() << " reading platform id...");
        ipmi::command::generic::response::GetDeviceId device_rsp{};
        bmc.ipmi().send(ipmi::command::generic::request::GetDeviceId{}, device_rsp);
        log_debug("bmc", bmc.get_id() << " platform id: 0x" << std::hex << static_cast<unsigned>(device_rsp.get_product_id()));
        return static_cast<std::uint16_t>(device_rsp.get_product_id());
    }
    catch (std::exception& e) {
        log_error("bmc", bmc.get_id() << " platform id read failed: " << e.what());
    }
    return {};
}


OptionalField<std::uint16_t> read_smbios_checksum(Bmc& bmc) {
    log_debug("bmc", bmc.get_id() << " reading smbios region status ");
    if (Bmc::State::ONLINE == bmc.get_state()) {
        try {
            auto mdr_accessor = ipmi::sdv::MdrRegionAccessorFactory()
                .create(bmc.get_platform_id(), bmc.ipmi(), ipmi::command::sdv::DataRegionId::SMBIOS_TABLE);
            return mdr_accessor->get_mdr_region_checksum();
        }
        catch (const std::exception& e) {
            log_warning("bmc", bmc.get_id() << " smbios region status read failed: " << e.what());
        }
    }
    return {};
}


void send_event(const std::string& module, const std::string& parent, Component type, Notification notification) {
    agent_framework::eventing::EventData edat;
    edat.set_parent(parent);
    edat.set_component(module);
    edat.set_type(type);
    edat.set_notification(notification);
    agent_framework::eventing::EventsQueue::get_instance()->push_back(edat);
    log_info("bmc", "Sending an event for component " << edat.get_component() << "," << "("
                                  << edat.get_type() << ")" << ", notification type: " << edat.get_notification());
}


void remove_processors(const std::string& system_uuid) {
    auto& pm = get_manager<agent_framework::model::Processor>();
    pm.remove_by_parent(system_uuid);
}


void remove_memory_modules(const std::string& system_uuid) {
    auto& mm = get_manager<agent_framework::model::Memory>();
    mm.remove_by_parent(system_uuid);
}


void remove_network_interfaces(const std::string& system_uuid) {
    auto& nicm = get_manager<agent_framework::model::NetworkInterface>();
    nicm.remove_by_parent(system_uuid);
}


void remove_storage_subsystems(const std::string& system_uuid) {
    auto& ssm = get_manager<agent_framework::model::StorageSubsystem>();
    auto& scm = get_manager<agent_framework::model::StorageController>();
    for (const auto& subsystem_uuid : ssm.get_keys(system_uuid)) {
        scm.remove_by_parent(subsystem_uuid);
    }
    ssm.remove_by_parent(system_uuid);
}

void remove_trusted_modules(const std::string& system_uuid) {
    auto& tmm = get_manager<agent_framework::model::TrustedModule>();
    tmm.remove_by_parent(system_uuid);
}

void remove_network_devices(const std::string& system_uuid) {
    auto& ndm = get_manager<agent_framework::model::NetworkDevice>();
    auto& ndfm = get_manager<agent_framework::model::NetworkDeviceFunction>();
    for (const auto& device_uuid : ndm.get_keys(system_uuid)) {
        ndfm.remove_by_parent(device_uuid);
    }
    ndm.remove_by_parent(system_uuid);
}


void remove_systems(const std::string& manager_uuid) {
    auto& sm = get_manager<agent_framework::model::System>();
    auto system_keys = sm.get_keys(manager_uuid);
    for (const auto& uuid : system_keys) {
        remove_processors(uuid);
        remove_memory_modules(uuid);
        remove_network_interfaces(uuid);
        remove_storage_subsystems(uuid);
        remove_network_devices(uuid);
        remove_trusted_modules(uuid);
    }

    sm.remove_by_parent(manager_uuid);
    for (const auto& uuid: system_keys) {
        // sending delete events for systems
        send_event(uuid, manager_uuid, Component::System, Notification::Remove);
    }
}

void remove_fans(const std::string& thermal_zone_uuid) {
    auto& fm = get_manager<agent_framework::model::Fan>();
    fm.remove_by_parent(thermal_zone_uuid);
}

void remove_thermal_zones(const std::string& chassis_uuid) {
    auto& tzm = get_manager<agent_framework::model::ThermalZone>();
    auto thermal_zone_keys = tzm.get_keys(chassis_uuid);
    for (const auto& uuid : thermal_zone_keys) {
        remove_fans(uuid);
    }
    tzm.remove_by_parent(chassis_uuid);
}

void remove_power_zones(const std::string& chassis_uuid) {
    auto& pzm = get_manager<agent_framework::model::PowerZone>();
    pzm.remove_by_parent(chassis_uuid);
}

void remove_drives(const std::string& chassis_uuid) {
    auto& dm = get_manager<agent_framework::model::Drive>();
    dm.remove_by_parent(chassis_uuid);
}

void remove_chassis(const std::string& manager_uuid) {
    auto& chm = get_manager<agent_framework::model::Chassis>();
    auto chassis_keys = chm.get_keys(manager_uuid);
    for (const auto& uuid : chassis_keys) {
        remove_drives(uuid);
        remove_thermal_zones(uuid);
        remove_power_zones(uuid);
    }

    chm.remove_by_parent(manager_uuid);
    for (const auto& uuid : chassis_keys) {
        // sending delete events for chassis
        send_event(uuid, manager_uuid, Component::Chassis, Notification::Remove);
    }
}

void remove_pcie_functions(const std::string& pcie_device_uuid) {
    auto& pfm = get_manager<agent_framework::model::PcieFunction>();
    pfm.remove_by_parent(pcie_device_uuid);
}

void remove_pcie_devices(const std::string& manager_uuid) {
    auto& pdm = get_manager<agent_framework::model::PcieDevice>();
    auto pcie_device_keys = pdm.get_keys(manager_uuid);
    for (const auto& uuid : pcie_device_keys) {
        remove_pcie_functions(uuid);
    }
    pdm.remove_by_parent(manager_uuid);
}

void remove_manager(const std::string& manager_uuid) {
    remove_systems(manager_uuid);
    remove_chassis(manager_uuid);
    remove_pcie_devices(manager_uuid);

    auto& mm = get_manager<agent_framework::model::Manager>();
    mm.remove_entry(manager_uuid);
    send_event(manager_uuid, {}, Component::Manager, Notification::Remove);
}


void update_status(const std::string& manager_uuid, agent_framework::model::enums::State state, Notification notification) {
    auto& chm = get_manager<agent_framework::model::Chassis>();
    auto chassis_keys = chm.get_keys(manager_uuid);

    for (const auto& key : chassis_keys) {
        auto chassis = chm.get_entry_reference(key);
        auto status = chassis->get_status();
        status.set_state(state);
        chassis->set_status(status);
        send_event(key, manager_uuid, Component::Chassis, notification);
    }

    auto& mm = get_manager<agent_framework::model::Manager>();
    auto manager = mm.get_entry_reference(manager_uuid);
    auto status = manager->get_status();
    status.set_state(state);
    manager->set_status(status);
    send_event(manager_uuid, {}, Component::Manager, notification);
}


class PeriodicDiscovery : public std::enable_shared_from_this<PeriodicDiscovery> {
public:
    PeriodicDiscovery(Bmc& bmc, Bmc::Duration period, Bmc::Duration smbios_wait_delay)
        : m_bmc(bmc), m_period(period), m_smbios_wait_time(smbios_wait_delay) {}


    void operator()() {
        if (Bmc::State::ONLINE == m_bmc.get_state()) {
            bool checksum_changed{false};
            const auto checksum = read_smbios_checksum(m_bmc);
            if (checksum.has_value()) {
                if (checksum != m_smbios_checksum) {
                    if (m_smbios_checksum.has_value()) {
                        log_info("periodic_discovery", m_bmc.get_id() << " SMBIOS checksum changed 0x"
                                << std::hex << uint(m_smbios_checksum) << " -> 0x" << uint(checksum));
                    }
                    else {
                        log_info("periodic_discovery", m_bmc.get_id() << " SMBIOS checksum: "
                                << std::hex << " 0x" << uint(checksum));
                    }
                    m_smbios_checksum = checksum;
                    checksum_changed = true;
                }
            }
            else {
                m_no_smbios_counter++;
            }

            if (checksum_changed || requires_discovery_when_smbios_not_present()) {
                do_discovery();
            }
            // reschedule task
            m_bmc(get_rerun_delay(), std::bind(&PeriodicDiscovery::operator(), shared_from_this()));
        }
    }

    ~PeriodicDiscovery() {
        log_info("periodic_discovery", m_bmc.get_id()
                << " stopped. # of discoveries run: " << m_discovery_counter);
    }

private:

    void do_discovery() {
        m_discovery_counter++;
        log_info("periodic_discovery", m_bmc.get_id() << " Starting discovery #" << m_discovery_counter);
        try {
            auto mdr_accessor_factory = std::make_shared<ipmi::sdv::MdrRegionAccessorFactory>();
            discovery::DiscoveryManager discovery_manager{m_bmc, mdr_accessor_factory};
            const auto stabilized_manager_uuid = discovery_manager.discover();
            m_bmc.set_manager_uuid(stabilized_manager_uuid);
        }
        catch (const std::exception& e) {
            log_error("periodic_discovery", m_bmc.get_id() << " Discovery Failed: " << e.what());
        }
    }

    Bmc::Duration get_rerun_delay() {
        if (is_discoverable_when_no_smbios() && !m_discovery_counter && (1 == m_no_smbios_counter)) {
            log_info("periodic_discovery", m_bmc.get_id() << " Waiting for SMBIOS... ");
            return m_smbios_wait_time;
        }
        return m_period;
    }

    // Discovery when no SMBIOS present will happen if:
    // 1. Platform is discoverable when no smbios present (ex. Grantley)
    // 2. No discovery done yet
    // 3. Already waited for smbios
    bool requires_discovery_when_smbios_not_present() const {
        return is_discoverable_when_no_smbios()
               && !m_discovery_counter
               && (m_no_smbios_counter > 1);
    }

    bool is_discoverable_when_no_smbios() const {
        using ipmi::command::generic::ProductId;
        return m_bmc.get_platform_id().has_value()
               && ProductId::PRODUCT_ID_INTEL_XEON_BDC_R == m_bmc.get_platform_id().value();
    }

    Bmc& m_bmc;
    const Bmc::Duration m_period;
    const Bmc::Duration m_smbios_wait_time;
    OptionalField<std::uint16_t> m_smbios_checksum{};
    uint m_no_smbios_counter{};
    uint m_discovery_counter{};
};

}


namespace agent {
namespace compute {

Bmc::Bmc(const ConnectionData& conn, Bmc::Duration state_update_interval,
         ReadPresenceFn read_presence, ReadOnlineStatusFn read_online_state)
    : agent_framework::Bmc(conn, state_update_interval, read_presence, read_online_state),
      m_ipmi{conn.get_ip_address(), conn.get_port(), conn.get_username(), conn.get_password()} {
}

bool Bmc::on_become_online(const Transition&) {
    m_platform_id = read_platform_id(*this);
    if (m_platform_id.has_value()) {
        m_telemetry_service = std::make_shared<ComputeTelemetryService>(ipmi(), m_platform_id);
        if (m_telemetry_service->get_reader_ptrs().empty()) {
            log_error("bmc", "No metrics defined for " << m_platform_id);
        }
    }
    const auto rerun_delay = std::chrono::seconds(10);
    const auto smbios_wait_time = std::chrono::minutes(3);
    (*this)(std::bind(&PeriodicDiscovery::operator(),
            std::make_shared<PeriodicDiscovery>(*this, rerun_delay, smbios_wait_time)));
    return true;
}

bool Bmc::on_extraction(const Transition&) {
    remove_manager(get_manager_uuid());
    m_platform_id.reset();
    m_telemetry_service.reset();
    return true;
}

bool Bmc::on_become_offline(const Transition& t) {
    if (Bmc::State::ONLINE == t.init_state) {
        m_telemetry_service.reset();
        try {
            remove_systems(get_manager_uuid());
            update_status(get_manager_uuid(), agent_framework::model::enums::State::UnavailableOffline, Notification::Update);
        }
        catch (const std::exception& e) {
            log_error("bmc", e.what());
        }
    }
    return true;
}

void Bmc::reset_tasks() {
    agent_framework::Bmc::reset_tasks();
    periodic_task("power_state_boot_options", Duration::zero(), get_update_interval(),
        [](Bmc& bmc) {
            if (Bmc::State::ONLINE == bmc.get_state()) {
                agent::compute::status::update_system_power_state(bmc);
                agent::compute::status::update_system_boot_options(bmc);
            }
        }, std::ref(*this));
    self_scheduling_task("telemetry_runner",
        [](Bmc& bmc, std::shared_ptr<telemetry::TelemetryRunner> ptr, Bmc::Duration duration) {
            return ptr->run(bmc, duration);
        }, std::ref(*this), std::make_shared<telemetry::TelemetryRunner>(), get_update_interval());
}

}
}
