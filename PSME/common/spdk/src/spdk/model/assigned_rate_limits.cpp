/*!
 * @brief AssignedRateLimits class implementation
 * @copyright Copyright (c) 2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions an
 * limitations under the License.
 *
 * @file assigned_rate_limits.cpp
 */

#include "spdk/model/assigned_rate_limits.hpp"
#include "spdk/json_optional.hpp"

using namespace spdk::model;

constexpr const char spdk::model::AssignedRateLimits::RW_MBYTES_PER_SEC[];
constexpr const char spdk::model::AssignedRateLimits::W_MBYTES_PER_SEC[];
constexpr const char spdk::model::AssignedRateLimits::R_MBYTES_PER_SEC[];
constexpr const char spdk::model::AssignedRateLimits::RW_IOS_PER_SEC[];


void spdk::model::from_json(const json::Json& json, AssignedRateLimits& assigned_rate_limits) {

    assigned_rate_limits.set_rw_mbytes_per_sec(
        json.value(AssignedRateLimits::RW_MBYTES_PER_SEC, json::Json{}).get<spdk::JsonOptional<std::uint64_t>>());
    assigned_rate_limits.set_r_mbytes_per_sec(
        json.value(AssignedRateLimits::R_MBYTES_PER_SEC, json::Json{}).get<spdk::JsonOptional<std::uint64_t>>());
    assigned_rate_limits.set_w_mbytes_per_sec(
        json.value(AssignedRateLimits::W_MBYTES_PER_SEC, json::Json{}).get<spdk::JsonOptional<std::uint64_t>>());
    assigned_rate_limits.set_rw_ios_per_sec(
        json.value(AssignedRateLimits::RW_IOS_PER_SEC, json::Json{}).get<spdk::JsonOptional<std::uint64_t>>());

}


void spdk::model::to_json(json::Json& json, const AssignedRateLimits& assigned_rate_limits) {

    json = json::Json{
        {AssignedRateLimits::RW_MBYTES_PER_SEC, assigned_rate_limits.get_rw_mbytes_per_sec()},
        {AssignedRateLimits::W_MBYTES_PER_SEC,  assigned_rate_limits.get_w_mbytes_per_sec()},
        {AssignedRateLimits::R_MBYTES_PER_SEC,  assigned_rate_limits.get_r_mbytes_per_sec()},
        {AssignedRateLimits::RW_IOS_PER_SEC,    assigned_rate_limits.get_rw_ios_per_sec()},
    };
}


std::string AssignedRateLimits::to_string() const {

    std::stringstream ss{};
    if (get_rw_mbytes_per_sec()) {
        ss << " Number of R/W megabytes per second " << get_rw_mbytes_per_sec().value() << std::endl;
    }
    if (get_r_mbytes_per_sec()) {
        ss << " Number of Read megabytes per second " << get_r_mbytes_per_sec().value() << std::endl;
    }
    if (get_w_mbytes_per_sec()) {
        ss << " Number of Write megabytes per second " << get_w_mbytes_per_sec().value() << std::endl;
    }
    if (get_rw_ios_per_sec()) {
        ss << " Number of R/W I/Os per second  " << get_rw_ios_per_sec().value() << std::endl;
    }

    return ss.str();
}




