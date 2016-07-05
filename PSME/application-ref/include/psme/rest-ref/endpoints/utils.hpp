/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * */

#ifndef APP_REST_ENDPOINT_UTILS_HPP
#define APP_REST_ENDPOINT_UTILS_HPP

#include "psme/rest-ref/server/parameters.hpp"
#include "psme/rest-ref/server/request.hpp"
#include "psme/rest-ref/server/response.hpp"
#include "path_builder.hpp"
#include "agent-framework/module-ref/enum/common.hpp"
#include "agent-framework/module-ref/model/resource.hpp"
#include "agent-framework/module-ref/model/manager.hpp"
#include "agent-framework/module-ref/model/chassis.hpp"
#include "agent-framework/module-ref/managers/generic_manager.hpp"
#include "agent-framework/module-ref/compute_manager.hpp"
#include "agent-framework/module-ref/network_manager.hpp"
#include "agent-framework/module-ref/chassis_manager.hpp"
#include "agent-framework/module-ref/storage_manager.hpp"
#include <vector>

namespace json {
    class Value;
}

namespace agent_framework { namespace model {
    class Resource;
} }

namespace psme {
namespace rest {
namespace endpoint {
namespace utils {

/*!
 * @brief Fetches uuid of port from given URL
 *
 * @param[in] port_url URL of the port
 *
 * @return uuid of the port
 */
std::string get_port_uuid_from_url(const std::string& port_url);

/*!
 * @brief make_endpoint_context
 *
 * @param[in] params request params
 * @param[in] link url
 *
 * @return context string
 */
std::string make_endpoint_context(const psme::rest::server::Parameters& params,
                                  std::string link);

/*!
 * @brief status_to_json get resource status (state and health)
 * and add it to GET reply json
 *
 * @param res a Resource
 * @param v the json value sent as a GET response
 */
void status_to_json(const agent_framework::model::Resource& res, json::Value& v);

/*!
 * @brief a method converting a string to uint64_t specialized for REST ids: if
 * the string is incorrect, the method throws an InvalidParameters exception
 *
 * @param id_as_string taken out from request url
 *
 * @return id as a number
 */
std::uint64_t id_to_uint64(const std::string& id_as_string);

/*!
 * @brief get_manager by rest id as string
 *
 * @param id_as_string taken out from request url
 *
 * @return copy of the Manager object
 */
agent_framework::model::Manager get_manager(const std::string& id_as_string);

/*!
 * @brief get manager by uuid
 *
 * @param uuid read from child
 *
 * @return copy of the Manager object
 */
agent_framework::model::Manager get_manager_by_uuid(const std::string& uuid);

/*!
 * @brief get_chassis by rest id as string
 *
 * @param id_as_string taken out from request url
 *
 * @return copy of the Chassis object
 */
agent_framework::model::Chassis get_chassis(const std::string& id_as_string);

/*!
 * @brief get chassis by uuid
 *
 * @param id_as_string read from another object
 *
 * @return copy of the Chassis object
 */
agent_framework::model::Chassis get_chassis_by_uuid(const std::string& id_as_string);

/*!
 * @brief get the ids of all Chassis that are children of Manager with the
 * given uuid
 *
 * @param manager_uuid id of a Manager, whose chassis collection we want to get
 * as ids
 *
 * @return a vector of rest ids
 */
std::vector<std::uint64_t> get_chassis_ids_by_parent(const std::string& manager_uuid);

/*!
 * @brief does the manager with this uuid have a chassis which is of Drawer
 * Type?
 *
 * @param manager_uuid the manager uuid
 *
 * @return is the manager a drawer manager
 */
bool is_drawer_manager(const std::string& manager_uuid);

/*!
 * @brief finds the part of the URL after "/redfish/v1" using
 * recursion
 *
 * @param type component's type
 *
 * @param uuid component's uuid
 *
 * @return component's REST URL, trailing part
 */
std::string get_component_url_recursive(
    agent_framework::model::enums::Component type,
    const std::string& uuid);

/*!
 * @brief finds the URL of the given component
 *
 * @param type component's type
 *
 * @param uuid component's uuid
 *
 * @return component's REST URL
 */
std::string get_component_url(agent_framework::model::enums::Component type,
    const std::string& uuid);

/*!
 * @brief Sets component location in response header
 *
 * @param res Response
 *
 * @param path component's REST URL
 */
void set_location_header(server::Response& res, const server::Request&,
        const std::string& path);

/*!
 * @brief query all resources of type Resource, with a given parent,
 * return vector of results
 *
 * @tparam Resource type of queried model resources
 * @tparam QueryResult return type of query function
 * @param parent_uuid parent of queried objects
 * @param query a function operating on model resources
 *
 * @return vector of query results
 */
template <typename Resource, typename QueryResult>
std::vector<QueryResult> query_entries(const std::string& parent_uuid,
        std::function<QueryResult(const Resource&)> query){
    std::vector<QueryResult> results{};
    for (const auto& resource_uuid :
            agent_framework::module::get_manager<Resource>()
                .get_keys(parent_uuid)) {
        auto resource = agent_framework::module::get_manager<Resource>()
                .get_entry(resource_uuid);
        results.emplace_back(query(resource));
    }
    return results;
}

/*!
 * @brief process all healths given in input for HealthRollup
 *
 * @param healths a vector (intension: initializer list) of Health enum objects
 *
 * @return the rolled-up health
 */
agent_framework::model::enums::Health general_health_rollup(
        const std::vector<agent_framework::model::enums::Health>& healths);

/*!
 * @brief simple (non-recursive) template used to roll-up the Health of all
 * children of type Child
 *
 * @tparam Parent the model class behind the endpoint
 * @tparam Child the model class whose members are queried for their health
 * @param parent reference to the perent for which the rollup is determined
 *
 * @return the rolled-up health
 */
template <typename Parent, typename Child>
agent_framework::model::enums::Health rollup_children_health(const Parent& parent) {
    using agent_framework::model::enums::Health;
    auto get_health = [](const Child& ch){return ch.get_status().get_health();};
    auto children_healths = query_entries<Child, Health> (parent.get_uuid(), get_health);
    return general_health_rollup(children_healths);
}

/*!
 * @brief simple (non-recursive) template used to fill the HealthRollup field in
 * REST endpoints taking into account children of one type
 *
 * @tparam Parent the model class behind the endpoint
 * @tparam Child the model class whose members are queried for their health
 * @param parent reference to the perent for which the rollup is determined
 *
 * @return the rolled-up health
 */
template <typename Parent, typename Child>
agent_framework::model::enums::Health rollup_health(const Parent& parent) {
    // add parent's own health to childrens' health rollup
    auto healths = std::vector<agent_framework::model::enums::Health>
        {rollup_children_health<Parent, Child>(parent),
            parent.get_status().get_health()};

    return general_health_rollup(healths);
}

/*!
 * @brief simple (non-recursive) template used to fill the HealthRollup field in
 * REST endpoints taking into account children of one type, and their children
 * of one type
 *
 * @tparam Parent the model class behind the endpoint
 * @tparam Child the model class whose members are queried for their health
 * @tparam GrandChild the model class whose members are queried via Children
 * @param parent reference to the perent for which the rollup is determined
 *
 * @return the rolled-up health
 */
template <typename Parent, typename Child, typename GrandChild>
agent_framework::model::enums::Health rollup_health(const Parent& parent) {
    using agent_framework::model::enums::Health;
    auto get_child_health_rollup = [](const Child& ch){return rollup_health<Child, GrandChild>(ch);};
    auto healths = query_entries<Child, Health> (parent.get_uuid(), get_child_health_rollup);
    healths.emplace_back(parent.get_status().get_health());
    return general_health_rollup(healths);
}

/*!
 * @brief rollup the health of a logical drive, recursively getting the health
 * of all children logical drives and their physical drives
 *
 * @param logical_drive_uuid the uuid of the querried logical drive
 *
 * @return the drive's HealthRollup
 */
agent_framework::model::enums::Health rollup_logical_drive_health(const std::string& logical_drive_uuid);


/*!
 * @brief rollup the health of a Remote Target, visiting all logical drives in
 * it's luns
 *
 * @param remote_target_uuid the uuid of the querried iscsi target
 *
 * @return the target's HealthRollup
 */
agent_framework::model::enums::Health rollup_remote_target_health(const std::string& remote_target_uuid);


/*!
 * @brief rollup the health of a Remote Target
 *
 * @param storage_service_uuid the uuid of the querried storage service
 *
 * @return the service's HealthRollup
 */
agent_framework::model::enums::Health rollup_storage_service_health(const std::string& storage_service_uuid);


/*!
 * @brief rollup the health of a Computer System
 *
 * @param system_uuid the uuid of the querried system
 *
 * @return the system's HealthRollup
 */
agent_framework::model::enums::Health rollup_system_health(const std::string& system_uuid);


/*!
 * @brief rollup the health of a Switch Port
 *
 * @param port_uuid the uuid of the querried port
 *
 * @return the port's HealthRollup
 */
agent_framework::model::enums::Health rollup_switch_port_health(const std::string& port_uuid);


/*!
 * @brief rollup the health of a Switch
 *
 * @param switch_uuid the uuid of the querried switch
 *
 * @return the switch's HealthRollup
 */
agent_framework::model::enums::Health rollup_switch_health(const std::string& switch_uuid);


/*!
 * @brief rollup the health of a Mananger
 *
 * @param manager_uuid the uuid of the querried manager
 *
 * @return the manager's HealthRollup
 */
agent_framework::model::enums::Health rollup_manager_health(const std::string&
    manager_uuid);

/*!
 * @brief convert value in GB to value in GiB
 * Used for Drives (Logical, Physical)
 *
 * @param quantity_in_gb the quantity to convert
 *
 * @return the quantity in GiB
 */
double gb_to_gib(double quantity_in_gb);

/*!
 * @brief convert value in GiB to value in GB
 * Used for POST on Logical Drives
 *
 * @param quantity_in_gib the quantity to convert
 *
 * @return the quantity in GB
 */
double gib_to_gb(double quantity_in_gib);

/*!
 * @brief convert value in MB to value in MiB
 * Used for Dimms
 *
 * @param quantity_in_mb the quantity to convert
 *
 * @return the quantity in MiB
 */
double mb_to_mib(double quantity_in_mb);

/*!
 * @brief convert value in MB to value in GiB
 * Used for Dimms
 *
 * @param quantity_in_mb the quantity to convert
 *
 * @return the quantity in GiB
 */
double mb_to_gib(double quantity_in_mb);

/*!
 * @brief convert value in MB to value in MiB
 * Region attribute in model uses uint32_t for capacity and offset
 *
 * @param quantity_in_mb the quantity to convert
 *
 * @return the quantity in MiB
 */
uint32_t mb_to_mib(uint32_t quantity_in_mb);

}
}
}
}

#endif
