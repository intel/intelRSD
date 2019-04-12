/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 *
 * @file discoverer.cpp
 *
 * */

#include "agent-framework/discovery/builders/identifier_builder.hpp"
#include "agent-framework/discovery/discoverers/system_discoverer.hpp"
#include "discovery/discoverer.hpp"
#include "fru_eeprom/fru_eeprom_parser.hpp"
#include "sysfs/sysfs_interface.hpp"



using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent::pnc::discovery::builders;
using namespace agent::pnc::discovery;
using namespace agent::pnc::tools;
using namespace agent::pnc::sysfs;
using namespace agent::pnc::nvme;
using namespace agent::pnc::gas;
using namespace agent::pnc::gas::mrpc;
using namespace agent::pnc;
using namespace fru_eeprom::parser;


Discoverer::~Discoverer() = default;


Chassis Discoverer::discover_chassis(const Toolset&, const Chassis& chassis) const {
    return m_factory->init_builder(m_factory->get_chassis_builder(), chassis)->build();
}


Fabric Discoverer::discover_fabric(const std::string& manager_uuid) const {
    return m_factory->init_builder(m_factory->get_fabric_builder(), manager_uuid)->build();
}


System Discoverer::discover_system(const std::string& manager_uuid,
                                   const std::string& chassis_uuid) const {
    auto sysfs = std::make_shared<::sysfs::SysfsInterface>(::sysfs::SysfsInterface{});
    return agent_framework::discovery::SystemDiscoverer(sysfs).discover(manager_uuid, chassis_uuid);
}


StorageSubsystem Discoverer::discover_storage_subsystem(const std::string& system_uuid) const {
    return m_factory->init_builder(m_factory->get_storage_subsystem_builder(), system_uuid)->build();
}


Switch Discoverer::discover_switch(const std::string& fabric_uuid, const Toolset& tools,
                                   const std::string& chassis_uuid, const SysfsSwitch& sysfs_switch) const {
    auto builder = m_factory->init_builder(m_factory->get_switch_builder(), fabric_uuid);
    builder->update_sysfs(sysfs_switch).update_links(chassis_uuid);

    Seeprom seeprom{};
    if (tools.i2c_tool->get_seeprom(seeprom)) {
        builder->update_seeprom(seeprom);
    }
    else {
        log_error("pnc-discovery", "Cannot read switch seeprom data");
    }
    return builder->build();
}


Zone Discoverer::discover_zone(const std::string& fabric_uuid, const tools::Toolset&,
                               const std::string& switch_uuid, GlobalAddressSpaceRegisters& gas,
                               uint8_t partition_id) const {
    auto builder = m_factory->init_builder(m_factory->get_zone_builder(), fabric_uuid);
    builder->update_id(partition_id).update_links(switch_uuid);
    try {
        gas.read_partition(partition_id);
        builder->update_pc(gas.partition);
    }
    catch (const std::exception& e) {
        log_error("pnc-discovery", "Cannot read Zone " << unsigned(partition_id) << " data. " << e.what());
    }
    return builder->build();
}


bool Discoverer::update_port(Port& port, const GlobalAddressSpaceRegisters& gas, const Toolset& tools) const {
    bool changed = false;
    try {
        // throw exception if anything went wrong
        auto lsr = tools.gas_tool->get_link_status(gas, uint8_t(port.get_phys_port_id()));

        uint32_t width{};
        uint32_t max_width{};
        double speed{};
        double max_speed = PM85X6_MAX_LINK_SPEED;
        std::tie(width, max_width, speed) =
            tools.map_tool->get_port_width_and_speed_from_link_status(lsr);
        // up to this point speed and max speed are in GT/s - they have to be converted into Gbps before going further
        // for that we need currently configured width and speed in GT/s
        changed |= tools.model_tool->set_port_width_and_speed(port,
                                                              width, max_width,
                                                              tools.map_tool->gtps_to_gbps(speed, width),
                                                              tools.map_tool->gtps_to_gbps(max_speed, width));

        changed |= tools.model_tool->set_status(port,
                                                tools.map_tool->get_port_status_from_link_status(lsr));
    }
    catch (const std::exception& e) {
        log_error("pnc-discovery", "Cannot update status on port " << port.get_uuid() <<
                                                                   ", exception: " << e.what());
        changed = tools.model_tool->set_status_offline(port);
    }
    return changed;
}


Port Discoverer::discover_port(const std::string& switch_uuid, const tools::Toolset& tools,
                               GlobalAddressSpaceRegisters& gas, const PortBindingInfo& cmd, uint8_t entry_id) const {
    auto builder = m_factory->init_builder(m_factory->get_port_builder(), switch_uuid);
    builder->update_binding_info(entry_id, cmd, gas.top);

    // get configuration data
    uint8_t port_id = uint8_t(builder->build().get_phys_port_id());
    Port tmp_port{};
    if (tools.model_tool->get_port_by_phys_id(tmp_port, port_id, true)) {
        builder->update_twi_config(tmp_port.get_twi_port(), tmp_port.get_twi_channel());
    }
    else {
        log_warning("pnc-discovery",
                    "Port configuration data not found for physical port:" << unsigned(port_id));
        throw PncDiscoveryExceptionNoConfiguration{};
    }

    // if host port -> update cable id and csr data
    if (builder->build().get_port_type() == enums::PciePortType::UpstreamPort) {
        CableId cable_id{m_platform};
        if (tools.i2c_tool->get_cable_id(cable_id, tmp_port)) {
            builder->update_cable_id(cable_id);
        }
        else {
            log_warning("pnc-discovery", "Reading CableId for Port " << unsigned(port_id) << " failed. ");
        }
    }
    return builder->build();
}


Endpoint Discoverer::discover_host_endpoint(const std::string& fabric_uuid) const {
    Endpoint endpoint = m_factory->init_builder(m_factory->get_endpoint_builder(), fabric_uuid)->add_host_entity().build();
    agent_framework::discovery::IdentifierBuilder::set_uuid(endpoint, endpoint.get_uuid());
    return endpoint;
}


Metric Discoverer::discover_port_health_metric(const agent_framework::model::Port& port, const Toolset& tools) const {
    auto builder = m_factory->init_builder(m_factory->get_metric_builder(), "");
    builder->update_resource(port).update_from_metric_definition(tools.model_tool->get_health_metric_definition())
        .update_value(port.get_status().get_health());
    return builder->build();
}


std::vector<MetricDefinition> Discoverer::discover_metric_definitions() const {
    std::vector<MetricDefinition> pnc_metric_definitions;

    auto builder = m_factory->init_builder(m_factory->get_metric_definition_builder(), "");

    builder->prepare_port_health_definition();
    pnc_metric_definitions.push_back(builder->build());

    return pnc_metric_definitions;
}



