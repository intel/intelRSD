/*!
 * @copyright
 * Copyright (c) 2016-2017 Intel Corporation
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

#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/pnc_components.hpp"
#include "agent-framework/eventing/events_queue.hpp"

namespace agent {
namespace pnc {
namespace tools {

/*! ModelTool class, set of the commonly used model operation */
class ModelTool {
public:

    /*! Default constructor */
    ModelTool() {}

    /*! Enable copy */
    ModelTool(const ModelTool&){}
    ModelTool& operator=(const ModelTool&) {return *this;}

    /*! Default desctructor */
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

    /*
     * @brief Gets zone by zone id
     * @param[out] zone Reference to the zone that will be filled with data
     * @param[in] switch_uuid Id of the zone's parent switch
     * @param[in] zone_id Id of the zone
     * @return True if successful
     * */
    virtual bool get_zone_by_id(agent_framework::model::Zone& zone, const std::string& switch_uuid,
        const std::uint32_t zone_id) const;

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
     * @brief Remove temporary ports from the model
     * */
    virtual void remove_temporary_ports() const {
        agent_framework::module::get_manager<agent_framework::model::Port>().remove_by_parent("");
    }

    /*!
     * @brief Sends event
     * @param[in] parent Parent uuid of the resource
     * @param[in] uuid Uuid of the resource
     * @param[in] component Type of the component
     * @param[in] type Type of the notification (add/remove/update)
     * */
    virtual void send_event(const std::string& parent, const std::string& uuid,
        agent_framework::model::enums::Component component, agent_framework::eventing::Notification type) const;

    /*!
     * @brief Updates status and media life left parameters of a given drive
     * @param[in] drive_uuid Uuid of the drive to be updated
     * @param[in] status New drive status
     * @param[in] media_life_left New media life left value
     * */
    virtual void update_drive_status(const std::string& drive_uuid,
        const agent_framework::model::attribute::Status& status, const int media_life_left) const;

    /*!
     * @brief Gets list of all drives that are visible on a specific dsp port
     * @param[in] port_uuid Uuid of the queried dsp port
     * @return Vector of all drives on the port
     * */
    virtual std::vector<std::string> get_drives_by_dsp_port_uuid(const std::string& port_uuid) const;

    /*!
     * @brief Gets list of all functions that are visible on a specific dsp port
     * @param[in] port_uuid Uuid of the queried dsp port
     * @return Vector of all functions on the port
     * */
    virtual std::vector<std::string> get_functions_by_dsp_port_uuid(const std::string& port_uuid) const;

    /*!
     * @brief Gets list of all endpoints that use specific drive_uuid
     * @param[in] drive_uuid Uuid of the drive
     * @return Vector of all endpoints using the drive
     * */
    virtual std::vector<std::string> get_endpoints_by_drive_uuid(const std::string& drive_uuid) const;

    /*!
     * @brief Removes drives from the entity list on the endpoint and sets it in a degenerate states
     * @param[in] endpoint_uuid Uuid of the endpoint to be updated
     * @param[in] drive_list Uuid of the drives to be removed
     * */
    virtual void degenerate_endpoint(const std::string& endpoint_uuid, const std::vector<std::string>& drive_list) const;

    /*!
    * @brief Adds drive to the entity list on the endpoint and sets it in a Enabled/OK states
    * @param[in] endpoint_uuid Uuid of the endpoint to be updated
    * @param[in] drive_uuid Uuid of the drive to be added
     * */
    virtual void regenerate_endpoint(const std::string& endpoint_uuid, const std::string& drive_uuid) const;

    /*!
     * @brief Gets ports by drive_uuid
     * @param[in] drive_uuid Uuid of the drive
     * @return Uuids of the ports used by the drive
     * */
    virtual std::vector<std::string> get_ports_by_drive_uuid(const std::string& drive_uuid) const;

    /*!
     * @brief Sets warning status override for a drive
     * @param[in] drive_uuid Uuid of the drive
     * @param[in] is_in_warning_state Override value
     * */
    virtual void set_drive_is_in_warning_state(const std::string& drive_uuid, bool is_in_warning_state) const;

    /*!
     * @brief Sets drive to be in 'being erased' state
     * @param[in] drive_uuid Uuid of the drive
     * @param[in] is_being_erased 'Being erased' flag value
     * */
    virtual void set_drive_is_being_erased(const std::string& drive_uuid, bool is_being_erased) const;

    /*!
     * @brief Sets the state of the drive
     * @param[in] drive_uuid Uuid of the drive
     * @param[in] status New status value
     * */
    virtual void set_drive_status(const std::string& drive_uuid,
        const agent_framework::model::attribute::Status& status) const;

    /*!
     * @brief Gets switch uuid for a given drive
     * @param[in] drive_uuid Uuid of the drive
     * @return Uuid of the switch
     * */
    virtual std::string get_switch_for_drive_uuid(const std::string& drive_uuid) const;

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
};

using ModelToolPtr = std::shared_ptr<ModelTool>;

}
}
}
