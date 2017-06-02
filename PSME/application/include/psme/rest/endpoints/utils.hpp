/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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

#pragma once



#include "path_builder.hpp"
#include "psme/rest/server/parameters.hpp"
#include "psme/rest/server/request.hpp"
#include "psme/rest/server/response.hpp"

#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/model/resource.hpp"
#include "agent-framework/module/model/manager.hpp"
#include "agent-framework/module/model/chassis.hpp"
#include "agent-framework/module/managers/generic_manager.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/compute_components.hpp"
#include "agent-framework/module/network_components.hpp"
#include "agent-framework/module/chassis_components.hpp"
#include "agent-framework/module/storage_components.hpp"
#include "agent-framework/module/pnc_components.hpp"
#include <vector>



namespace json {
class Value;
}

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
 * @brief a method converting a string to uint64_t specialized for REST ids: if
 * the string is incorrect, the method throws an InvalidParameters exception
 *
 * @param id_as_string taken out from request url
 *
 * @return id as a number
 */
std::uint64_t id_to_uint64(const std::string& id_as_string);


/*!
 * @brief does the manager with this uuid have a chassis which is of Drawer or Enclosure type?
 *
 * @param manager_uuid the manager uuid
 *
 * @return is the manager a drawer or enclosue manager
 */
bool is_manager_for_drawer_or_enclosure(const std::string& manager_uuid);


/*!
 * @brief finds the part of the URL after "/redfish/v1" using
 * recursion
 *
 * @param path the pathbuilder that carries the url under construction
 *
 * @param type component's type
 *
 * @param uuid component's uuid
 */
void get_component_url_recursive(psme::rest::endpoint::PathBuilder& path, agent_framework::model::enums::Component type,
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
std::string get_component_url(agent_framework::model::enums::Component type, const std::string& uuid);


/*!
 * @brief Sets component location in response header
 *
 * @param res Response
 *
 * @param path component's REST URL
 */
void set_location_header(server::Response& res, const std::string& path);


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
template<typename Resource, typename QueryResult>
std::vector<QueryResult> query_entries(const std::string& parent_uuid,
                                       std::function<QueryResult(const Resource&)> query) {
    std::vector<QueryResult> results{};
    for (const auto& resource_uuid : agent_framework::module::get_manager<Resource>().get_keys(parent_uuid)) {
        auto resource = agent_framework::module::get_manager<Resource>().get_entry(resource_uuid);
        results.emplace_back(query(resource));
    }
    return results;
}


/*!
 * @brief Round double-precision number to 2-digits precision
 * @param number Value to round precesion
 * @param digits Number of digits
 * @return Rounded value
 */
double round(double number, int digits);


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


/*!
 * @brief convert value in GB to value in B
 * used in Drives under Chassis
 *
 * @param quantity_in_gb the quantity to convert
 *
 * @return the quantity in B
 */
uint64_t gb_to_b(double quantity_in_gb);


/*!
 * @brief compare the resources which are currently under a parent resource in many-to-many-manager
 * with resources requested in eg. a PATCH request.
 * Determine which resources should be added or removed to comply with the PATCH request.
 *
 * @param manager the querried many-to-many manager
 * @param parent_uuid parent resource uuid
 * @param requested_children the new children requested with PATCH method
 * @param children_to_add the children that have to be added to the parent
 * @param children_to_remove the children that have to be removed from the parent
 */
void children_to_add_to_remove(agent_framework::module::managers::ManyToManyManager& manager,
                               const std::string& parent_uuid,
                               const std::vector<std::string>& requested_children,
                               std::vector<std::string>& children_to_add,
                               std::vector<std::string>& children_to_remove);
}
}
}
}

