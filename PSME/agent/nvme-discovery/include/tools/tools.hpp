/*!
 * @brief Definition of Tools class.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
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
 * @file tools.hpp
 */

#pragma once

#include "agent-framework/module/model/endpoint.hpp"
#include "agent-framework/module/model/storage_pool.hpp"

#include <string>
#include <set>

namespace agent {
namespace nvme {
namespace tools {

/*!
 * @brief Checks if given endpoint is in target role.
 *
 * @param endpoint Endpoint to check
 * @return true if endpoint is in target role
 */
bool is_target(const agent_framework::model::Endpoint& endpoint);

/*!
 * @brief Checks if given endpoint is in initiator role.
 *
 * @param endpoint Endpoint to check
 * @return true if endpoint is in initiator role
 */
bool is_initiator(const agent_framework::model::Endpoint& endpoint);

/*!
 * @brief Converts nqn to subnqn.
 *
 * @param nqn proper format nqn ex: nqn.2014-08.org.nvmexpress:uuid:ffffffff-ffff-ffff-ffff-ffffffffffff
 */
void convert_to_subnqn(std::string& nqn);

/*!
 * @brief Disconnect from target endpoint.
 *
 * @param target Endpoint that is assumed to be target.
 */
void disconnect_from_target(const agent_framework::model::Endpoint& target);

/*!
 * @brief Disconnect all target endpoints in given zone.
 *
 * @param zone Zone's UUID.
 */
void disconnect_all_targets(const Uuid& zone);

/*!
 * @brief Get drive/volume name from system path.
 *
 * @param path system path (i.e. /dev/nvme1)
 * @return drive/volume name (i.e. nvme1)
 */
std::string get_name_from_path(const std::string& path);

/*!
 * @brief Updates storage pool consumed capacity
 *
 * @param storage_pool_uuid Uuid of the storage pool to be updated
 */
void update_storage_pool_consumed_capacity(const Uuid& storage_pool_uuid);

/*!
 * @brief Check if given container has duplicates.
 *
 * Useful for checking if a given request contains duplicate uuids.
 *
 * @param container The container with comparable values.
 * @return true if the container has duplicates, false otherwise.
 *
 */
template<typename T>
bool has_duplicates(const T& container) {
    std::set<typename T::value_type> no_duplicates(container.begin(), container.end());
    return no_duplicates.size() != container.size();
}

}
}
}
