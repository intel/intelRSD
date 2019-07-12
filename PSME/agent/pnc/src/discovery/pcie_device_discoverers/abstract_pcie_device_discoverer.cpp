/*!
 * @brief Implementation of common methods for abstract class for discovery of PCIe devices
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation
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
 * @file abstract_pcie_device_discoverer.cpp
 */

#include "agent-framework/database/database_entities.hpp"
#include "agent-framework/discovery/builders/identifier_builder.hpp"
#include "agent-framework/eventing/utils.hpp"
#include "agent-framework/module/model/attributes/event_data.hpp"
#include "agent-framework/module/managers/utils/manager_utils.hpp"
#include "configuration/configuration.hpp"
#include "discovery/discovery_exceptions.hpp"
#include "discovery/discovery_utils.hpp"
#include "discovery/device_discoverers/abstract_pcie_device_discoverer.hpp"
#include "logger/logger.hpp"
#include "loader/pnc_configuration.hpp"
#include "sysfs/sysfs_decoder.hpp"
#include "sysfs/sysfs_reader.hpp"
#include "tools/database_keys.hpp"



using namespace agent::pnc::discovery::builders;
using namespace agent::pnc::discovery;
using namespace agent::pnc::tools;
using namespace agent::pnc;
using namespace agent::pnc::discovery::device_discoverers;
using namespace agent_framework::model;
using namespace agent::pnc::sysfs;


bool
AbstractPcieDeviceDiscoverer::oob_port_device_discovery(const gas::GlobalAddressSpaceRegisters& gas,
                                                        const Uuid& dsp_port_uuid) const {

    log_debug("pnc-discovery", get_device_type_name() + " discovery: getting switch data...");

    try {

        log_debug("pnc-discovery",
                  get_device_type_name() + " discovery: getting " + get_device_type_name() + " data...");

        auto device_uuid = discover_oob_add_and_stabilize(dsp_port_uuid);

        sync_device_properties_with_db(device_uuid);

        discover_target_endpoint(gas, device_uuid, dsp_port_uuid);

        return true;
    }
    catch (agent::pnc::discovery::PncDiscoveryExceptionDeviceNotFound&) {

        auto dsp_port = agent_framework::module::get_manager<agent_framework::model::Port>().get_entry(dsp_port_uuid);

        log_debug("pnc-discovery",
                  "No " + get_device_type_name() + " detected on physical port " << dsp_port.get_port_id());

        return false;
    }
}


bool
AbstractPcieDeviceDiscoverer::ib_port_device_discovery(const Uuid& switch_uuid,
                                                       const Uuid& dsp_port_uuid,
                                                       uint8_t bridge_id,
                                                       const Uuid& device_uuid) const {

    try {

        // sysfs numbering is different than in pnc, in pnc bridge_id = 0 is reserved for upstream ports
        if (0 == bridge_id) {
            THROW(agent_framework::exceptions::PncError, "pnc-discovery", "Invalid bridge id = 0");
        }

        bridge_id = uint8_t(bridge_id - 1);

        SysfsDecoder decoder = SysfsDecoder::make_instance(SysfsReader{});

        Switch pcie_switch = agent_framework::module::get_manager<Switch>().get_entry(switch_uuid);

        SysfsBridge sysfs_bridge = decoder.get_bridge_by_switch_path(pcie_switch.get_bridge_path(), bridge_id);

        log_debug("pnc-discovery", "Discovery: bridge discovery...");

        std::vector<SysfsDevice> devices = decoder.get_devices(sysfs_bridge);

        if (devices.empty()) {

            if (device_uuid.empty()) {

                // no oob drive + no ib devices -> nothing new detected
                log_debug("pnc-discovery", "Device not present!");
            }
            else {

                // device was found via OOB discovery and devices are empty -> this should never happen
                // setting device in permanent critical state
                critical_state_device_discovery(device_uuid);
            }
        }
        else if (devices.size() > 1) {

            // more than one device - should never happen
            log_debug("pnc-discovery", "Found too many (" << devices.size()
                                                          << ") devices on port uuid = "
                                                          << dsp_port_uuid);

            log_error("pnc-discovery", "Too many PCIe devices found on port!");

            throw std::runtime_error("Too many devices on port");
        }
        else {

            SysfsDevice sysfsdevice = devices.front();

            auto discovered_device_uuid = sysfs_device_discovery(dsp_port_uuid, device_uuid, decoder, sysfsdevice);

            if (discovered_device_uuid.empty()) {
                // no device detected but sysfs device found
                log_warning("pnc-discovery", "Unsupported device found!");
            }
            else {
                // normal situation
                sysfs_discovery(discovered_device_uuid, sysfsdevice);
            }
        }

        return true;
    }
    catch (const PncDiscoveryExceptionDeviceNotFound&) {
        return false;
    }

    catch (const std::exception& e) {
        log_error("pnc-discovery", "Error during discovery: " << e.what());
        return false;
    }
}


agent_framework::model::PcieDevice
AbstractPcieDeviceDiscoverer::discover_pcie_device(const Uuid& manager_uuid,
                                                   const Uuid& chassis_uuid,
                                                   const agent::pnc::sysfs::SysfsDevice& sysfs_device) const {

    return m_factory->init_builder(m_factory->get_pcie_device_builder(),
                                   manager_uuid)->update_sysfs(sysfs_device).update_links(chassis_uuid).build();
}


agent_framework::model::PcieFunction
AbstractPcieDeviceDiscoverer::discover_pcie_function(const Uuid& device_uuid,
                                                     const Uuid& dsp_port_uuid,
                                                     const SysfsFunction& sysfs_function) const {

    return m_factory->init_builder(m_factory->get_pcie_function_builder(), device_uuid)->update_sysfs(
        sysfs_function).update_links(dsp_port_uuid).build();
}


void
AbstractPcieDeviceDiscoverer::update_endpoint_zone_binding(const gas::GlobalAddressSpaceRegisters& gas,
                                                           const std::string& endpoint_uuid,
                                                           const agent_framework::model::Port& port) const {

    const auto& endpoint_discovery_mode = loader::PncConfiguration::get_instance()->get_fabric_discovery_mode();

    auto fabric_uuid = m_tools.model_tool->get_fabric_uuid();

    if (endpoint_discovery_mode == loader::FabricDiscoveryMode::AUTOMATIC) {

        gas::mrpc::PortBindingInfo pbi = m_tools.gas_tool->get_port_binding_info(gas, uint8_t(port.get_phys_port_id()));

        for (unsigned i = 0; i < pbi.output.fields.info_count; ++i) {

            if (pbi.output.fields.port_binding_info[0].partition_id != gas.top.output.fields.current_partition_id) {

                agent_framework::model::Zone zone{};
                m_tools.model_tool->get_zone_by_id(zone, port.get_parent_uuid(),
                                                   pbi.output.fields.port_binding_info[0].partition_id);

                agent_framework::module::get_m2m_manager<agent_framework::model::Zone, agent_framework::model::Endpoint>().add_entry(
                    zone.get_uuid(), endpoint_uuid);

                agent_framework::eventing::send_event(zone.get_uuid(),
                                                      agent_framework::model::enums::Component::Zone,
                                                      agent_framework::model::enums::Notification::Update,
                                                      fabric_uuid);
            }
        }
    }
    else {

        agent_framework::database::FabricEntity fabric_db{fabric_uuid};
        agent_framework::database::EndpointEntity endpoint_db{endpoint_uuid};

        auto zones_uuids_db = fabric_db.get_multiple_values(db_keys::ZONES);
        auto port_uuid_db = endpoint_db.get(db_keys::DEVICE_PORT);

        if (port_uuid_db == port.get_uuid()) {

            agent::pnc::discovery::utils::update_endpoint_zone_binding_from_db(zones_uuids_db, endpoint_uuid);
        }
    }
}


Uuid
AbstractPcieDeviceDiscoverer::add_and_stabilize_endpoint(agent_framework::model::Endpoint& endpoint,
                                                         const agent_framework::model::Port& port,
                                                         const std::string& device_uuid) const {

    Uuid endpoint_uuid = endpoint.get_uuid();

    const auto& endpoint_discovery_mode = loader::PncConfiguration::get_instance()->get_fabric_discovery_mode();

    if (endpoint_discovery_mode == loader::FabricDiscoveryMode::AUTOMATIC) {

        PncStabilizer stabilizer{};

        endpoint_uuid =
            stabilizer.dry_stabilize(endpoint, std::vector<agent_framework::model::Port>{port});
    }
    else {
        agent_framework::module::get_m2m_manager<Endpoint, Port>().add_entry(endpoint_uuid, port.get_uuid());
    }

    bool already_exists =
        agent_framework::module::get_manager<agent_framework::model::Endpoint>().entry_exists(endpoint_uuid);

    std::string fabric_uuid = m_tools.model_tool->get_fabric_uuid();

    if (already_exists) {
        if (is_recognized_device()) {
            log_info("pnc-discovery", "Regenerating existing endpoint");
            log_debug("pnc-discovery", "Regenerating endpoint on a physical port " << port.get_port_id());

            m_tools.model_tool->regenerate_endpoint(endpoint_uuid, device_uuid);
        }
        else {
            log_debug("pnc-discovery", "Endpoint already exists");
        }

    }
    else {
        log_debug("pnc-discovery", "New endpoint has been found on a physical port " << port.get_port_id());

        agent_framework::module::get_m2m_manager<agent_framework::model::Endpoint, agent_framework::model::Port>()
            .add_entry(endpoint.get_uuid(), port.get_uuid());

        agent_framework::discovery::IdentifierBuilder::set_uuid(endpoint, endpoint_uuid);
        log_and_add(endpoint);

        agent_framework::eventing::send_event(PncTreeStabilizer().stabilize_pcie_endpoint(endpoint.get_uuid()),
                                              agent_framework::model::enums::Component::Endpoint,
                                              agent_framework::model::enums::Notification::Add,
                                              fabric_uuid);
    }

    return endpoint_uuid;
}


Uuid
AbstractPcieDeviceDiscoverer::sysfs_device_discovery(const Uuid& dsp_port_uuid,
                                                     const Uuid& device_uuid,
                                                     const agent::pnc::sysfs::SysfsDecoder& decoder,
                                                     const agent::pnc::sysfs::SysfsDevice& sysfs_device) const {

    log_debug("pnc-discovery", "Begin sysfs device discovery");

    std::string manager_uuid = m_tools.model_tool->get_manager_uuid();
    std::string chassis_uuid = m_tools.model_tool->get_chassis_uuid();

    // discover pcie device and functions
    PcieDevice device = discover_pcie_device(manager_uuid, chassis_uuid, sysfs_device);
    std::vector<SysfsFunction> sysfs_functions = decoder.get_functions(sysfs_device);

    log_and_add(device);

    for (const auto& sysfs_function : sysfs_functions) {

        PcieFunction function = discover_pcie_function(device.get_uuid(), dsp_port_uuid, sysfs_function);

        set_functional_device(function, device_uuid);

        log_and_add(function);
    }

    agent_framework::eventing::send_event(::agent::pnc::PncTreeStabilizer().stabilize_pcie_device(device.get_uuid()),
                                          enums::Component::PcieDevice,
                                          agent_framework::model::enums::Notification::Add,
                                          manager_uuid);

    return device_uuid;
}


void AbstractPcieDeviceDiscoverer::discover_target_endpoint(const gas::GlobalAddressSpaceRegisters& gas,
                                                            const Uuid& device_uuid, const Uuid& dsp_port_uuid) const {

    Endpoint endpoint{};
    std::string fabric_uuid = m_tools.model_tool->get_fabric_uuid();

    const auto& endpoint_discovery_mode = loader::PncConfiguration::get_instance()->get_fabric_discovery_mode();

    if (endpoint_discovery_mode == loader::FabricDiscoveryMode::AUTOMATIC) {

        endpoint = m_factory->init_builder(m_factory->get_endpoint_builder(), fabric_uuid)
            ->add_device_entity(device_uuid).build();
    }
    else {

        endpoint = recreate_target_endpoint_from_db(fabric_uuid, device_uuid, dsp_port_uuid);
    }

    if (!endpoint.get_connected_entities().empty()) {

        auto dsp_port = agent_framework::module::get_manager<agent_framework::model::Port>().get_entry(dsp_port_uuid);

        auto endpoint_uuid = add_and_stabilize_endpoint(endpoint, dsp_port, device_uuid);

        update_endpoint_zone_binding(gas, endpoint_uuid, dsp_port);
    }
}


agent_framework::model::Endpoint AbstractPcieDeviceDiscoverer::recreate_target_endpoint_from_db(const Uuid& fabric_uuid,
                                                                                                const Uuid& device_uuid,
                                                                                                const Uuid& port_uuid) const {

    log_debug("pnc-discovery", "Recreate target endpoint from db");

    Endpoint endpoint{};

    agent_framework::database::FabricEntity fabric_db{fabric_uuid};

    auto endpoints_uuids = fabric_db.get_multiple_values(db_keys::ENDPOINTS);

    for (const auto& endpoint_uuid : endpoints_uuids) {

        agent_framework::database::EndpointEntity endpoint_db{endpoint_uuid};

        auto role_db = endpoint_db.get(db_keys::ENDPOINT_ROLE);

        if (literals::Endpoint::TARGET == role_db) {

            log_debug("pnc-discovery", "Found target endpoint in fabric db, endpoint uuid: " << endpoint_uuid);

            auto device_uuid_db = endpoint_db.get(db_keys::DEVICE);
            auto endpoint_port_uuid_db = endpoint_db.get(db_keys::DEVICE_PORT);

            if (device_uuid == device_uuid_db || port_uuid == endpoint_port_uuid_db) {

                EndpointBuilder endpoint_builder;
                endpoint_builder.init(fabric_uuid);

                endpoint = endpoint_builder.add_device_entity(device_uuid_db).build();
                endpoint.set_uuid(endpoint_uuid);
                endpoint.set_status(attribute::Status(enums::State::Enabled, enums::Health::OK));

                agent_framework::discovery::IdentifierBuilder::set_uuid(endpoint, endpoint_uuid);

                log_and_add<Endpoint>(endpoint);

                log_debug("pnc-discovery", "Created target endpoint from fabric db, endpoint uuid: " << endpoint_uuid);

                break;
            }
        }
    }

    return endpoint;
}
