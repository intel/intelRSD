/*!
 * @brief Provides class for stabilizing compute agent resource tree
 *
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License") {}
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
 * @file update_relations.cpp
 * */

#include "tree_stability/helpers/update_relations.hpp"

#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/pnc_components.hpp"
#include "agent-framework/module/managers/utils/manager_utils.hpp"



using namespace agent_framework::model;
using namespace agent_framework::module;

namespace agent {
namespace pnc {
namespace helpers {

void update_pcie_switch_in_relations(const std::string& pcie_switch_temporary_uuid,
                                     const std::string& pcie_switch_persistent_uuid) {
    auto& zone_manager = get_manager<Zone>();

    auto zone_filer = [&pcie_switch_temporary_uuid](const Zone& zone) -> bool {
        return zone.get_switch_uuid() == pcie_switch_temporary_uuid;
    };

    const auto& zone_keys = zone_manager.get_keys(zone_filer);

    for (const auto& zone_key : zone_keys) {
        zone_manager.get_entry_reference(zone_key)->set_switch_uuid(pcie_switch_persistent_uuid);
    }
}


void update_pcie_endpoint_in_relations(const std::string& pcie_endpoint_temporary_uuid,
                                       const std::string& pcie_endpoint_persistent_uuid) {
    get_m2m_manager<Zone, Endpoint>().update_child(pcie_endpoint_temporary_uuid,
                                                   pcie_endpoint_persistent_uuid);
    get_m2m_manager<Endpoint, Port>().update_parent(pcie_endpoint_temporary_uuid,
                                                    pcie_endpoint_persistent_uuid);

    auto& endpoint_manager = get_manager<Endpoint>();
    auto endpoint = endpoint_manager.get_entry(pcie_endpoint_persistent_uuid);
    auto identifiers = endpoint.get_identifiers();
    for (auto& identifier : identifiers) {
        if (pcie_endpoint_temporary_uuid == identifier.get_durable_name()) {
            identifier.set_durable_name(pcie_endpoint_persistent_uuid);
        }
    }
    endpoint_manager.get_entry_reference(pcie_endpoint_persistent_uuid)->set_identifiers(std::move(identifiers));
}


void update_pcie_zone_in_relations(const std::string& pcie_zone_temporary_uuid,
                                   const std::string& pcie_zone_persistent_uuid) {
    get_m2m_manager<Zone, Endpoint>().update_parent(pcie_zone_temporary_uuid,
                                                    pcie_zone_persistent_uuid);
}


void update_pcie_port_in_relations(const std::string& pcie_port_temporary_uuid,
                                   const std::string& pcie_port_persistent_uuid) {
    auto& function_manager = get_manager<PcieFunction>();
    auto& drive_manager = get_manager<Drive>();

    auto function_filter = [&pcie_port_temporary_uuid](const PcieFunction& function) -> bool {
        return function.get_dsp_port_uuid() == pcie_port_temporary_uuid;
    };
    auto drive_filter = [&pcie_port_temporary_uuid](const Drive& drive) -> bool {
        const auto& dsp_ports = drive.get_dsp_port_uuids();
        return (std::find(dsp_ports.begin(), dsp_ports.end(), pcie_port_temporary_uuid) != dsp_ports.end());
    };

    const auto& functions_keys = function_manager.get_keys(function_filter);
    const auto& drives_keys = drive_manager.get_keys(drive_filter);

    for (const auto& function_key : functions_keys) {
        function_manager.get_entry_reference(function_key)->set_dsp_port_uuid(pcie_port_persistent_uuid);
    }
    for (const auto& drive_key : drives_keys) {
        auto drive = drive_manager.get_entry_reference(drive_key);
        std::vector<std::string> dsp_port_uuids = drive->get_dsp_port_uuids();
        for (auto& dsp_port_uuid : dsp_port_uuids) {
            if (dsp_port_uuid == pcie_port_temporary_uuid) {
                dsp_port_uuid = pcie_port_persistent_uuid;
            }
        }
        drive->set_dsp_port_uuids(dsp_port_uuids);
    }

    get_m2m_manager<Endpoint, Port>().update_child(pcie_port_temporary_uuid,
                                                   pcie_port_persistent_uuid);
}


void update_chassis_in_relations(const std::string& chassis_temporary_uuid,
                                 const std::string& chassis_persistent_uuid) {
    auto& switch_manager = get_manager<Switch>();
    auto& system_manager = get_manager<System>();
    auto& pcie_device_manager = get_manager<PcieDevice>();

    auto switch_filter = [&chassis_temporary_uuid](const Switch& pcie_switch) {
        return pcie_switch.get_chassis() == chassis_temporary_uuid;
    };

    auto system_filter = [&chassis_temporary_uuid](const System& system) {
        return system.get_chassis() == chassis_temporary_uuid;
    };

    auto pcie_device_filter = [&chassis_temporary_uuid](const PcieDevice& device) {
        return device.get_chassis() == chassis_temporary_uuid;
    };

    const auto& switch_keys = switch_manager.get_keys(switch_filter);
    const auto& system_keys = system_manager.get_keys(system_filter);
    const auto& device_keys = pcie_device_manager.get_keys(pcie_device_filter);

    for (const auto& switch_key : switch_keys) {
        switch_manager.get_entry_reference(switch_key)->set_chassis(chassis_persistent_uuid);
    }

    for (const auto& system_key : system_keys) {
        system_manager.get_entry_reference(system_key)->set_chassis(chassis_persistent_uuid);
    }

    for (const auto& device_key : device_keys) {
        pcie_device_manager.get_entry_reference(device_key)->set_chassis(chassis_persistent_uuid);
    }
}


void update_pcie_drive_in_relations(const std::string& pcie_drive_temporary_uuid,
                                    const std::string& pcie_drive_persistent_uuid) {
    auto& endpoint_manager = get_manager<Endpoint>();
    auto endpoint_filter = [&pcie_drive_temporary_uuid](const Endpoint& endpoint) {
        const auto& connected_entities = endpoint.get_connected_entities();
        for (const auto& connected_entity : connected_entities) {
            const auto& entity = connected_entity.get_entity();
            if (entity.has_value() && entity.value() == pcie_drive_temporary_uuid) {
                return true;
            }
        }
        return false;
    };
    const auto& endpoints_keys = endpoint_manager.get_keys(endpoint_filter);

    for (const auto& endpoint_key : endpoints_keys) {
        auto endpoint = endpoint_manager.get_entry_reference(endpoint_key);
        Endpoint::ConnectedEntities connected_entities = endpoint->get_connected_entities();
        for (auto& connected_entity : connected_entities) {
            if (connected_entity.get_entity().value() == pcie_drive_temporary_uuid) {
                connected_entity.set_entity(pcie_drive_persistent_uuid);
            }
        }
        endpoint->set_connected_entities(connected_entities);
    }

    auto& function_manager = get_manager<PcieFunction>();
    auto function_filter = [&pcie_drive_temporary_uuid](const PcieFunction& function) {
        if (function.get_functional_device().has_value() &&
            function.get_functional_device().value() == pcie_drive_temporary_uuid) {
            return true;
        }
        return false;
    };
    const auto& function_keys = function_manager.get_keys(function_filter);

    for (const auto& function_key : function_keys) {
        function_manager.get_entry_reference(function_key)->set_functional_device(pcie_drive_persistent_uuid);
    }

    get_m2m_manager<Drive, PcieFunction>().update_parent(pcie_drive_temporary_uuid, pcie_drive_persistent_uuid);
    get_m2m_manager<StorageSubsystem, Drive>().update_child(pcie_drive_temporary_uuid, pcie_drive_persistent_uuid);
}


void update_processor_in_relations(const std::string& processor_temporary_uuid,
                                   const std::string& processor_persistent_uuid) {
    auto& endpoint_manager = get_manager<Endpoint>();
    auto endpoint_filter = [&processor_temporary_uuid](const Endpoint& endpoint) {
        const auto& connected_entities = endpoint.get_connected_entities();
        for (const auto& connected_entity : connected_entities) {
            const auto& entity = connected_entity.get_entity();
            if (entity.has_value() && entity.value() == processor_temporary_uuid) {
                return true;
            }
        }
        return false;
    };
    const auto& endpoints_keys = endpoint_manager.get_keys(endpoint_filter);

    for (const auto& endpoint_key : endpoints_keys) {
        auto endpoint = endpoint_manager.get_entry_reference(endpoint_key);
        Endpoint::ConnectedEntities connected_entities = endpoint->get_connected_entities();
        for (auto& connected_entity : connected_entities) {
            if (connected_entity.get_entity().value() == processor_temporary_uuid) {
                connected_entity.set_entity(processor_persistent_uuid);
            }
        }
        endpoint->set_connected_entities(connected_entities);
    }

    auto& function_manager = get_manager<PcieFunction>();
    auto function_filter = [&processor_temporary_uuid](const PcieFunction& function) {
        if (function.get_functional_device().has_value() &&
            function.get_functional_device().value() == processor_temporary_uuid) {
            return true;
        }
        return false;
    };
    const auto& function_keys = function_manager.get_keys(function_filter);

    for (const auto& function_key : function_keys) {
        function_manager.get_entry_reference(function_key)->set_functional_device(processor_persistent_uuid);
    }
    PncComponents::get_instance()->get_processor_function_manager().update_parent(processor_temporary_uuid,
                                                                                  processor_persistent_uuid);
}


void update_storage_subsystem_in_relations(const std::string& storage_subsystem_temporary_uuid,
                                           const std::string& storage_subsystem_persistent_uuid) {
    get_m2m_manager<StorageSubsystem, Drive>().update_parent(
        storage_subsystem_temporary_uuid, storage_subsystem_persistent_uuid);
}

}
}
}
