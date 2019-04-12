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
 * @file lag.hpp
 *
 * @brief LAG utils
 * */

#pragma once

#include <string>
#include <vector>

namespace agent {
namespace network {
namespace utils {

/*!
 * @brief Invalid port speed
 * */
constexpr int INVALID_PORT_SPEED = -1;

/*!
 * @brief Add new logical port
 *
 * @param[in] lag_identifier Logical port identifier
 * @param[in] switch_uuid Switch UUID
 *
 * @return New logical port UUID
 * */
std::string add_logical_port(const std::string& lag_identifier,
                                    const std::string& switch_uuid);

/*!
 * @brief Check if port is member of LAG
 *
 * @param[in] port_identifier Physical port identifier
 *
 * @return true if port is member of LAG otherwise false
 * */
bool is_member_port(const std::string& port_identifier);

/*!
 * @brief Check if LAG is empty
 *
 * @param[in] lag_identifier Logical port identifier
 *
 * @return true if LAG is empty otherwise false
 * */
bool is_logical_port_empty(const std::string& lag_identifier);

/*!
 * @brief Get logical port members
 *
 * @param[in] lag_identifier Logical port identifier
 *
 * @return LAG port members
 * */
std::vector<std::string> get_logical_port_members(const std::string& lag_identifier);

/*!
 * @brief Get LAG indentifier
 *
 * @param[in] port_identifier Physical port identifier
 *
 * @return LAG identifier
 * */
std::string get_lag_identifier(const std::string& port_identifier);

/*!
 * @brief Validate port members.
 *
 * @param[in] ports List of ports to validate.
 * @param[in] lag_port_speed Speed of the port under the Lag.
 *                           INVALID_PORT_SPEED indicates no ports under
 *                           the Lag.
 * */
void validate_port_members(const std::vector<std::string>& ports,
                                    int lag_port_speed = INVALID_PORT_SPEED);

}
}
}
