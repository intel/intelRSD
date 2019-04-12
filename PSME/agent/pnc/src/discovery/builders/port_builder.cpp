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
 * @file port_builder.cpp
 *
 * @brief PortBuilder implementation
 * */

#include "discovery/builders/port_builder.hpp"
#include "discovery/discovery_utils.hpp"

using namespace agent::pnc::discovery::builders;
using namespace agent_framework::model;
using namespace agent::pnc::discovery::utils;
using namespace agent::pnc::gas::csr;
using namespace agent::pnc::gas::mrpc;
using namespace agent::pnc::gas::top;
using namespace agent::pnc::tools;

void PortBuilder::build_default() {
    m_obj.set_protocol(enums::TransportProtocol::PCIe);

    attribute::Status status;
    status.set_health(enums::Health::OK);
    status.set_state(enums::State::Enabled);
    m_obj.set_status(std::move(status));

    m_obj.add_collection(attribute::Collection(
        enums::CollectionName::Endpoints,
        enums::CollectionType::Endpoints
    ));
}

PortBuilder::ReturnType PortBuilder::update_cable_id(const CableId& cable_id) {
    if (m_obj.get_port_type().has_value() && m_obj.get_port_type() == enums::PciePortType::UpstreamPort) {
        auto serial_number = get_null_terminated_string(cable_id.fields.serial_number,
            CableId::HOST_CABLE_EEPROM_SERIAL_NUMBER_SIZE_BYTES);
        m_obj.add_cable_id(serial_number);
        return *this;
    }
    log_error("pnc-discovery", "Tried adding cable Id for non host port");
    throw std::runtime_error("Adding cable Ids for non-host ports");
}

PortBuilder::ReturnType PortBuilder::update_binding_info(const std::uint32_t entry_id, const PortBindingInfo& cmd,
        const TopLevelRegisters& top) {

    std::uint32_t phys_port_id = std::uint32_t(cmd.output.fields.port_binding_info[entry_id].phy_port_id);
    m_obj.set_port_id(std::to_string(phys_port_id));
    m_obj.set_phys_port_id(phys_port_id);

    /* Management Port. Port is used to internal processing or management access.
     * Port has to be in current partition and upstream (Logical Bridge ID = 0) */
    if (0 == cmd.output.fields.port_binding_info[entry_id].logical_bridge_id
        && top.output.fields.current_partition_id == cmd.output.fields.port_binding_info[entry_id].partition_id) {
        m_obj.set_port_type(enums::PciePortType::ManagementPort);
    }
    /*! Host Port. Port is upstream. Compute hosts are connected to such ports.*/
    else if (0 == cmd.output.fields.port_binding_info[entry_id].logical_bridge_id) {
        m_obj.set_port_type(enums::PciePortType::UpstreamPort);
    }
    /* Device Port. Port is downstream */
    else {
        m_obj.set_port_type(enums::PciePortType::DownstreamPort);
    }
    return *this;
}

PortBuilder::ReturnType PortBuilder::update_twi_config(uint32_t twi_port, uint32_t twi_channel) {
    m_obj.set_twi_port(twi_port);
    m_obj.set_twi_channel(twi_channel);
    return *this;
}
