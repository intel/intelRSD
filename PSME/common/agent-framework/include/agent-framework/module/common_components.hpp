/*!
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
 *
 * @file common_components.hpp
 * @brief CommonComponents Interface
 * */

#pragma once



#include "agent-framework/generic/singleton.hpp"

#include "agent-framework/module/managers/generic_manager.hpp"
#include "agent-framework/module/managers/many_to_many_manager.hpp"
#include "agent-framework/module/managers/utils/manager_utils.hpp"
#include "agent-framework/module/model/model_common.hpp"



namespace agent_framework {
namespace module {

/*!
 * Class for managing common components
 * */
class CommonComponents final : public agent_framework::generic::Singleton<CommonComponents> {
public:
    using ModuleManager = GenericManager<model::Manager>;
    using ChassisManager = GenericManager<model::Chassis>;
    using DriveManager = GenericManager<model::Drive>;
    using SystemManager = GenericManager<model::System>;
    using StorageSubsystemManager = GenericManager<model::StorageSubsystem>;
    using TaskManager = GenericManager<model::Task>;
    using NetworkInterfaceManager = GenericManager<model::NetworkInterface>;
    using StorageDrivesManager = managers::ManyToManyManager;
    using StoragePoolVolumesManager = managers::ManyToManyManager;
    using ProcessorManager = GenericManager<model::Processor>;
    using MetricManager = GenericManager<model::Metric>;
    using MetricDefinitionManager = GenericManager<model::MetricDefinition>;
    using FabricManager = GenericManager<model::Fabric>;
    using ZoneManager = GenericManager<model::Zone>;
    using EndpointManager = GenericManager<model::Endpoint>;
    using PcieDeviceManager = GenericManager<model::PcieDevice>;
    using PcieFunctionManager = GenericManager<model::PcieFunction>;
    using ZoneEndpointManager = managers::ManyToManyManager;
    using StorageServiceDrivesManager = managers::ManyToManyManager;

    virtual ~CommonComponents();


    /*!
     * Get module manager
     *
     * @return Reference to module manager
     * */
    ModuleManager& get_module_manager() {
        return m_module_manager;
    }


    /*!
     * Get chassis manager
     *
     * @return Reference to chassis manager
     * */
    ChassisManager& get_chassis_manager() {
        return m_chassis_manager;
    }


    /*!
     * Get drive manager
     *
     * @return Reference to drive manager
     * */
    DriveManager& get_drive_manager() {
        return m_drive_manager;
    }


    /*!
     * Get task manager
     *
     * @return Reference to task manager
     * */
    TaskManager& get_task_manager() {
        return m_task_manager;
    }

    /*!
     * Get metric manager
     *
     * @return Reference to metric manager
     * */
    MetricManager& get_metric_manager() {
        return m_metric_manager;
    }

    /*!
     * Get metric definition manager
     *
     * @return Reference to metric definition manager
     * */
    MetricDefinitionManager& get_metric_definition_manager() {
        return m_metric_definition_manager;
    }

    /*!
     * @brief Get network interface manager
     * @return Instance of network interface manager
     * */
    NetworkInterfaceManager& get_network_interface_manager() {
        return m_network_interface_manager;
    }


    /*!
     * @brief Get Storage Subsystem manager
     * @return Reference to a storage Subsystem manager instance
     * */
    StorageSubsystemManager& get_storage_subsystem_manager() {
        return m_storage_subsystem_manager;
    }

    /*!
     * @brief Get StorageService Drives manager
     * @return Reference to a storage service drives manager
     * */
    StorageServiceDrivesManager& get_storage_service_drives_manager() {
        return m_storage_service_drives_manager;
    }



    /*!
     * @brief Get system manager
     * @return Instance of system manager
     * */
    SystemManager& get_system_manager() {
        return m_system_manager;
    }


    /*!
     * @brief Get manager for StorageSubsystem - Drive mappings
     * @return manager for StorageSubsystem - Drive mappings
     * */
    StorageDrivesManager& get_storage_subsystem_drives_manager() {
        return m_storage_subsystem_drives_manager;
    }


    /*!
     * @brief Get manager for StoragePool - Volume mappings
     * @return Manager for StoragePool - Volume mappings
     * */
    StoragePoolVolumesManager& get_storage_pool_volumes_manager() {
        return m_storage_pool_volumes_manager;
    }

    /*!
     * @brief Get processor manager
     * @return Instance of processor manager
     * */
    ProcessorManager& get_processor_manager() {
        return m_processor_manager;
    }

    /*!
     * Get fabric manager
     *
     * @return Reference to fabric manager
     * */
    FabricManager& get_fabric_manager() {
        return m_fabric_manager;
    }

    /*!
     * @brief Get manager for Zones
     * @return manager for Zones
     * */
    ZoneManager& get_zone_manager() {
        return m_zone_manager;
    }

    /*!
     * @brief Get manager for Endpoints
     * @return manager for Endpoints
     * */
    EndpointManager& get_endpoint_manager() {
        return m_endpoint_manager;
    }

    /*!
     * @brief Get manager for PcieDevices
     * @return manager for PcieDevices
     * */
    PcieDeviceManager& get_pcie_device_manager() {
        return m_pcie_device_manager;
    }

    /*!
     * @brief Get manager for PcieFunctions
     * @return manager for PcieFunctions
     * */
    PcieFunctionManager& get_pcie_function_manager() {
        return m_pcie_function_manager;
    }

    /*!
     * @brief Get manager for Zone-Endpoint mappings
     * @return manager for Zone-Endpoint mappings
     * */
    ZoneEndpointManager& get_zone_endpoint_manager() {
        return m_zone_endpoint_manager;
    }


private:
    ModuleManager m_module_manager{};
    ChassisManager m_chassis_manager{};
    DriveManager m_drive_manager{};
    TaskManager m_task_manager{};
    NetworkInterfaceManager m_network_interface_manager{};
    SystemManager m_system_manager{};
    StorageSubsystemManager m_storage_subsystem_manager{};
    StorageServiceDrivesManager m_storage_service_drives_manager{};
    StorageDrivesManager m_storage_subsystem_drives_manager{};
    StoragePoolVolumesManager m_storage_pool_volumes_manager{};
    MetricManager m_metric_manager{};
    ProcessorManager m_processor_manager{};
    MetricDefinitionManager m_metric_definition_manager{};
    FabricManager m_fabric_manager{};
    ZoneManager m_zone_manager{};
    EndpointManager m_endpoint_manager{};
    PcieDeviceManager m_pcie_device_manager{};
    PcieFunctionManager m_pcie_function_manager{};
    ZoneEndpointManager m_zone_endpoint_manager{};
};

}
}
