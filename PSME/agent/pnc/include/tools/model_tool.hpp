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
 *
 * @file model_tool.hpp
 *
 * @brief ModelTool class containing common model operations - reading/updating
 * and other common model operations.
 * */

#pragma once



#include "agent-framework/database/database_entity.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/pnc_components.hpp"
#include "agent-framework/eventing/events_queue.hpp"
#include "database_keys.hpp"
#include "tree_stability/pnc_stabilizer.hpp"
#include "gas/global_address_space_registers.hpp"



#define PNC_SUPPORTED_DEVICE agent_framework::model::Drive, agent_framework::model::Processor

namespace agent {
namespace pnc {
namespace tools {

/*! ModelTool class, set of the commonly used model operation */
class ModelTool {
public:

    /*! Default constructor */
    ModelTool() {}


    /*! Enable copy */
    ModelTool(const ModelTool&) {}


    ModelTool& operator=(const ModelTool&) { return *this; }


    /*! Default destructor */
    virtual ~ModelTool();


    /*!
     * @brief Method used to get Port object from the model using its physical Id
     * @param[out] port Reference to the port used as a return value
     * @param[in] phy_port_id Physical port id
     * @param[in] is_temporary True if search is done for temporary ports (with "" as a parent Uuid), default = false
     * @return True if successful
     * */
    virtual bool get_port_by_phys_id(agent_framework::model::Port& port, const std::uint32_t phy_port_id,
                                     bool is_temporary = false) const;


    /*!
     * @brief Gets zone by zone id
     * @param[out] zone Reference to the zone that will be filled with data
     * @param[in] switch_uuid Id of the zone's parent switch
     * @param[in] zone_id Id of the zone
     * @return True if successful
     * */
    virtual bool get_zone_by_id(agent_framework::model::Zone& zone, const std::string& switch_uuid,
                                const std::uint32_t zone_id) const;


    /*!
     * @brief Get a copy of the MetricDefinition for Port Health metric
     * @return Copy of the definition object
     */
    virtual agent_framework::model::MetricDefinition get_health_metric_definition() const;


    /*!
     * @brief Gets the uuid of the main manager of the agent
     * @return Uuid of the manager
     * */
    virtual std::string get_manager_uuid() const;


    /*!
     * @brief Gets the uuid of the main chassis of the manager
     * @return Uuid of the chassis
     * */
    virtual std::string get_chassis_uuid() const;


    /*!
     * @brief Gets the uuid of the main fabric of the manager
     * @return Uuid of the fabric
     * */
    virtual std::string get_fabric_uuid() const;


    /*!
     * @brief Gets the uuid of the main system of the manager
     * @return Uuid of the system
     * */
    virtual std::string get_system_uuid() const;


    /*!
     * @brief Gets the uuid of the main storage subsystem of the system
     * @return Uuid of the storage
     * */
    virtual std::string get_storage_uuid() const;


    /*!
     * @brief Gets the uuid of the main PCI switch
     * @return Uuid of the switch
     * */
    virtual std::string get_switch_uuid() const;


    /*!
     * @brief Gets dry-stabilized uuid of the main chassis of the manager.
     * @return dry-stabilized UUID of the chassis
     */
    Uuid get_dry_stabilized_chassis_uuid() const {
        PncStabilizer stabilizer;
        auto chassis_resource = agent_framework::module::get_manager<agent_framework::model::Chassis>().get_entries().front();
        auto switch_resource = agent_framework::module::get_manager<agent_framework::model::Switch>().get_entries().front();
        return stabilizer.dry_stabilize(chassis_resource, switch_resource);
    }


    /*!
     * @brief Gets dry-stabilized uuid of the main system of the manager
     * @return dry-stabilized UUID of the system
     */
    Uuid get_dry_stabilized_system_uuid() const {
        PncStabilizer stabilizer;
        auto system = agent_framework::module::get_manager<agent_framework::model::System>().get_entries().front();
        return stabilizer.dry_stabilize(system);
    }


    /*!
     * @brief Remove temporary ports from the model
     * */
    virtual void remove_temporary_ports() const {
        agent_framework::module::get_manager<agent_framework::model::Port>().remove_by_parent("");
    }


    /*!
     * @brief Updates status and media life left parameters of a given drive
     * @param[in] drive_uuid Uuid of the drive to be updated
     * @param[in] status New drive status
     * @param[in] media_life_left New media life left value
     * */
    virtual void update_drive_status(const std::string& drive_uuid,
                                     const agent_framework::model::attribute::Status& status,
                                     const int media_life_left) const;


    /*!
     * @brief Gets list of all devices of specified type that are visible on a specific dsp port
     * @tparam Device Type of device
     * @param[in] port_uuid Uuid of the queried dsp port
     * @return Vector of UUIDs of all devices of specified type on the port
     * */
    template<typename Device>
    std::vector<Uuid> get_devices_by_dsp_port_uuid(const Uuid& port_uuid) const {
        auto is_device_on_dsp_port = [&port_uuid](const Device& device) -> bool {
            const auto& dsp_ports = device.get_dsp_port_uuids();
            return (std::find(dsp_ports.begin(), dsp_ports.end(), port_uuid) != dsp_ports.end());
        };

        return agent_framework::module::get_manager<Device>().get_keys(is_device_on_dsp_port);
    }


    /*!
     * @brief Gets list of all devices of specified type that are visible on a specific dsp port
     * @tparam Device Type of device
     * @tparam NextDevice Type of device
     * @tparam Devices Types of device
     * @param[in] port_uuid Uuid of the queried dsp port
     * @return Vector of UUIDs of all devices of specified type on the port
     * */
    template<typename Device, typename NextDevice, typename ...Devices>
    std::vector<Uuid> get_devices_by_dsp_port_uuid(const Uuid& port_uuid) const {
        std::vector<Uuid> devices = get_devices_by_dsp_port_uuid<Device>(port_uuid);
        std::vector<Uuid> more_devices = get_devices_by_dsp_port_uuid<NextDevice, Devices...>(port_uuid);

        devices.insert(devices.end(), more_devices.begin(), more_devices.end());
        return devices;
    }


    std::vector<Uuid> get_devices_by_dsp_port_uuid(const Uuid& port_uuid) const {
        return get_devices_by_dsp_port_uuid<PNC_SUPPORTED_DEVICE>(port_uuid);
    }


    /*!
     * @brief Gets list of all functions that are visible on a specific dsp port
     * @param[in] port_uuid Uuid of the queried dsp port
     * @return Vector of all functions on the port
     * */
    virtual std::vector<std::string> get_functions_by_dsp_port_uuid(const std::string& port_uuid) const;


    /*!
     * @brief Gets list of all endpoints that use specific device_uuid
     * @param[in] device_uuid Uuid of the device
     * @return Vector of all endpoints using the device
     * */
    virtual std::vector<Uuid> get_endpoints_by_device_uuid(const Uuid& device_uuid) const;


    /*!
     * @brief Removes devices from the entity list on the endpoint and sets it in a degenerate states
     * @param[in] endpoint_uuid Uuid of the endpoint to be updated
     * @param[in] device_list Uuid of the devices to be removed
     * */
    virtual void degenerate_endpoint(const Uuid& endpoint_uuid, const std::vector<Uuid>& device_list) const;


    /*!
    * @brief Adds drive to the entity list on the endpoint and sets it in a Enabled/OK states
    * @param[in] endpoint_uuid Uuid of the endpoint to be updated
    * @param[in] drive_uuid Uuid of the drive to be added
     * */
    virtual void regenerate_endpoint(const std::string& endpoint_uuid, const std::string& drive_uuid) const;


    /*!
     * @brief Gets ports by drive_uuid
     * @param[in] device_uuid Uuid of the drive
     * @return Uuids of the ports used by the drive
     * */
    virtual std::vector<Uuid> get_ports_by_device_uuid(const Uuid& device_uuid) const;


    /*!
     * @brief Gets ports associated with endpoint_uuid
     * @param[in] endpoint_uuid Uuid of the endpoint
     * @return Uuids of the ports associated with endpoint_uuid
     * */
    virtual std::vector<Uuid> get_ports_uuids_by_endpoint_uuid(const Uuid& endpoint_uuid) const;


    /*!
     * @brief Sets warning status override for a device
     * @param[in] uuid Uuid of the device
     * @param[in] is_in_warning_state Override value
     * */
    template<typename Device>
    void set_device_in_warning_state(const std::string& uuid, bool is_in_warning_state) const {
        agent_framework::module::get_manager<Device>().get_entry_reference(uuid)->set_is_in_warning_state(
            is_in_warning_state);
    }


    /*!
     * @brief Sets device to be in 'being erased' state
     * @param[in] uuid Uuid of the device
     * @param[in] is_being_erased 'Being erased' flag value
     */
    template<typename Device>
    void set_device_being_erased(const std::string& uuid, bool is_being_erased) const {
        agent_framework::module::get_manager<Device>().get_entry_reference(uuid)->set_is_being_erased(is_being_erased);
    }


    /*!
     * @brief Sets device to be in 'being discovered' state
     * @tparam Device Type of PCIe device
     * @param[in] device_uuid Uuid of the device
     * @param[in] is_being_discovered 'Being discovered' flag value
     * @return True if status was changed.
     */
    template<typename Device>
    bool set_device_is_being_discovered(const Uuid& device_uuid, bool is_being_discovered) const {
        auto& manager = agent_framework::module::get_manager<Device>();
        if (manager.entry_exists(device_uuid)) {
            manager.get_entry_reference(device_uuid)->set_is_being_discovered(is_being_discovered);
            return true;
        }
        return false;
    }


    /*!
     * @brief Sets device to be in 'erased'
     * @param[in] uuid Uuid of the device
     * @param[in] is_erased determines whether the drive is erased
     */
    template<typename Device>
    void set_device_erased(const std::string& uuid, bool is_erased) {
        agent_framework::module::get_manager<Device>().get_entry_reference(uuid)->set_erased(is_erased);
    }


    /*!
     * @brief Sets device to be in 'being discovered' state
     * @tparam Device Type of device
     * @tparam NextDevice Type of device
     * @tparam Devices Types of device
     * @param[in] device_uuid Uuid of the device
     * @param[in] is_being_discovered 'Being discovered' flag value
     * @return True if status was changed.
     * */
    template<typename Device, typename NextDevice, typename ...Devices>
    bool set_device_is_being_discovered(const Uuid& device_uuid, bool is_being_discovered) const {
        bool status_changed = set_device_is_being_discovered<Device>(device_uuid, is_being_discovered);
        if (!status_changed) {
            return set_device_is_being_discovered<NextDevice, Devices...>(device_uuid, is_being_discovered);
        }
        return false;
    }


    /*!
     * @brief Sets Drive or Processor to be in 'being discovered' state
     * @param[in] device_uuid Uuid of the device
     * @param[in] is_being_discovered 'Being discovered' flag value
     * @return True if status was changed.
     */
    virtual bool set_device_is_being_discovered(const Uuid& device_uuid, bool is_being_discovered) const {
        return set_device_is_being_discovered<PNC_SUPPORTED_DEVICE>(device_uuid, is_being_discovered);
    }


    /*!
     * @brief Sets the state of the drive
     * @param[in] drive_uuid Uuid of the drive
     * @param[in] status New status value
     * */
    virtual void set_drive_status(const std::string& drive_uuid,
                                  const agent_framework::model::attribute::Status& status) const;


    /*!
     * @brief Gets switch uuid for a given drive or processor
     * @param[in] uuid Uuid of device
     * @return Uuid of the switch
     * */
    virtual std::string get_switch_for_device_uuid(const Uuid& uuid) const;


    /*!
     * @brief Update resource status
     * @param resource modified asset
     * @param status requested status
     * @return true if port was modified (status was changed)
     */
    virtual bool set_status(agent_framework::model::Resource& resource,
                            agent_framework::model::attribute::Status status) const;


    /*!
     * Update resource status to offline
     * @param resource modified asset
     * @return true if port was modified (status was changed)
     */
    virtual bool set_status_offline(agent_framework::model::Resource& resource) const;


    /*!
     * @brief Update port properties
     * @param port modified port
     * @param width current width
     * @param max_width maximal width
     * @param speed current speed
     * @param max_speed max speed
     * @return true if port was modified (anything was changed)
     */
    virtual bool set_port_width_and_speed(agent_framework::model::Port& port,
                                          uint32_t width, uint32_t max_width,
                                          double speed, double max_speed) const;


    /*!
     * @brief Checks if device with UUID device_uuid exists in model as a resource of any of specified types.
     * @tparam Device PCIe device type
     * @tparam NextDevice PCIe device type
     * @tparam Devices PCIe device type
     * @param device_uuid UUID of the device
     * @return True if device exists in model as a supported PCIe device, false otherwise
     */
    template<typename Device, typename NextDevice, typename ...Devices>
    bool exists_in_components(Uuid device_uuid) {
        return exists_in_components<Device>(device_uuid) || exists_in_components<NextDevice, Devices...>(device_uuid);
    }


    /*!
     * @brief Checks if device with UUID device_uuid exists in model as a resource of specified type.
     * @tparam Device PCIe device type
     * @param device_uuid UUID of the device
     * @return True if device exists in model as a supported PCIe device, false otherwise
     */
    template<typename Device>
    bool exists_in_components(Uuid device_uuid) {
        return agent_framework::module::get_manager<Device>().entry_exists(device_uuid);
    }


    template<typename Device>
    void set_device_status(const Uuid& uuid,
                           const agent_framework::model::attribute::Status& status) const {
        agent_framework::module::get_manager<Device>().get_entry_reference(uuid)->set_status(status);
    }


    /*!
     * @brief Checks if device with UUID device_uuid exists in model as a PCIe device supported by PNC.
     * @param device_uuid UUID of the device
     * @return True if device exists in model as a supported PCIe device, false otherwise
     */
    bool exists_in_components(Uuid device_uuid) {
        return exists_in_components<PNC_SUPPORTED_DEVICE>(device_uuid);
    }


    /*!
     * @brief Gets Endpoints with specified EntityRole from a list of specified Endpoints
     * @param endpoints_uuids Endpoints to filter from
     * @param role Role of Endpoints
     * @return Array of UUIDs of Endpoints which are in endpoints_uuids parameter and have specified role
     * @warning This function assumes there is only one ConnectedEntity per Endpoint.
     */
    std::vector<Uuid> get_endpoints_with_role(const agent_framework::model::attribute::Array<Uuid>& endpoints_uuids,
                                              const agent_framework::model::enums::EntityRole role);


    template<typename Resource> using ModelDatabase = agent_framework::database::DatabaseEntity<agent::pnc::tools::db_keys::RESOURCE_NAME<Resource>>;


    /*!
     * @brief Updates device's erased flag in model and in database
     * @tparam Device type of the device supported by PNC agent
     * @param device_uuid UUID of the device to update erased flag in
     * @param erased new value of erased flag
     */
    template<typename Device>
    void update_device_erased(const std::string& device_uuid, bool erased) {
        agent_framework::module::get_manager<Device>().get_entry_reference(device_uuid)->set_erased(erased);
        ModelDatabase<Device>(device_uuid).put(agent::pnc::tools::db_keys::DEVICE_ERASED_PROPERTY<Device>,
                                               std::to_string(static_cast<unsigned>(erased)));
    }


    /*!
     * @brief Synchronizes device properties with database device properties
     * @tparam Device type of the device supported by PNC agent
     * @tparam DeviceParent type of the device parent
     * @param parent_uuid uuid of the parent of device
     * @param device reference to device object from agent model
     */
    template<typename Device, typename DeviceParent>
    void sync_device_with_db(const Uuid& parent_uuid, Device& device) {

        auto db_uuids = ModelDatabase<DeviceParent>(parent_uuid).get_multiple_values(db_keys::DEVICES_PROPERTY<Device>);

        auto device_in_db = std::find_if(std::begin(db_uuids), std::end(db_uuids),
                                         [&device](const auto& db_uuid) {
                                             return db_uuid == device.get_uuid();
                                         }) != std::end(db_uuids);
        if (device_in_db) {
            bool erased = ModelDatabase<Device>(device.get_uuid()).get(db_keys::DEVICE_ERASED_PROPERTY<Device>) == "1";
            device.set_erased(erased);
        }
        else {
            ModelDatabase<Device>(device.get_uuid()).put(db_keys::DEVICE_ERASED_PROPERTY<Device>, std::to_string(
                static_cast<unsigned>(device.get_erased())));
            ModelDatabase<DeviceParent>(parent_uuid).append(db_keys::DEVICES_PROPERTY<Device>, device.get_uuid());
        }
    }


    /*!
     * @brief Removes device from database
     * @tparam Device type of the device supported by PNC agent
     * @tparam DeviceParent type of the device parent
     * @param parent_uuid UUID of the parent of device
     * @param device_uuid UUID of the device to remove from database
     */
    template<typename Device, typename DeviceParent>
    void remove_device_from_db(const Uuid& device_uuid, const Uuid& parent_uuid) {
        ModelDatabase<DeviceParent>(parent_uuid).del(device_uuid);
        ModelDatabase<Device>::remove(device_uuid);
        log_debug("pnc-agent", std::string(Device::get_component().to_string()) + "[UUID = " + device_uuid +
                               "] removed from database");
    }


    /*!
     * @brief Removes undiscovered devices from database. These devices are devices which exists
     * in database (e.g. from previous agent run) but were not discovered during current agent run.
     */
    void remove_undiscovered_devices_from_db();
};

using ModelToolPtr = std::shared_ptr<ModelTool>;

}
}
}
